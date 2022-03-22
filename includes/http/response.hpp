/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:52 by user42            #+#    #+#             */
/*   Updated: 2022/03/22 23:41:01 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../config/Config.hpp"
#include "../socket/Epoll.hpp"

#include "request.hpp"
#include "headersFunctions.hpp"
#include "mimeTypes.hpp"
#include "autoindex.hpp"

#include <fstream>
#include <cstdio>

#include <dirent.h>
#include <sys/types.h>


class httpResponse{
	public:
		httpResponse(){
			return;
		}

		~httpResponse(){
			return;
		}

		void	sendResponse(EpollSocket &socketEvent){
			socketEvent.sendData(this->_response);
		}

		void	buildResponse(httpRequest const &request, Config const &config){
			(void)config;
			this->_request = request;
			this->_status = request.getStatus();

			//need to locat where i am from the server root to adapt the path
			//this->_pathToRoot = this->_request.getRootPath() + this->_request.getPath() + "\r\n";
			this->_pathToRoot = "../../../../..";
			this->_pathToRoot.append(this->_request.getPath());

			//Methods
			if (this->_request.getMethod() == "POST")
				this->_uploadContent();
			else if (this->_request.getMethod() == "DELETE")
				this->_delete();
			this->_retrieveContent();

			//Build the response
			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();

			std::cout << std::endl << "SERVER RESPONSE :" << std::endl;
			std::cout << this->_response << std::endl;
		}

	private:

		//Build response
		void _buildStatusLine(){
			this->_response.append("HTTP/1.1 ");
			this->_response.append(itos(this->_status) + " ");
			this->_response.append(getStatusMessage(this->_status) + "\r\n");
		}

		void _buildHeaders(){
			//Mandatory HEADER (even if error)
			this->_response.append("Server: 42webserv/0.0.1\r\n");//need to get the server name
			this->_response.append("Date: " + getActualTime() + "\r\n");
			this->_response.append("Content-Type: " + this->_contentType + "\r\n");
			this->_response.append("Content-Length: " + itos(this->_content.size()) + "\r\n");
			this->_response.append("Connection: keep-alive\r\n"); //close or keep-alive make an enum
			//OPTIONNAL HEADERS (depending on method used if no error occurs)
			if (this->_status / 100 == 2 || this->_status / 100 == 3){
				this->_response.append("Last-Modified: " + getFileModification(this->_pathToRoot) + "\r\n");
				this->_response.append("Etag: " + makeETag(this->_pathToRoot) + "\r\n");
				//this->_response.append("Last-Modified: " + getFileModification(this->_request.getRootPath() + this->_request.getPath() + "\r\n");
				//this->_response.append("Etag: " + makeETag(this->_request.getRootPath() + this->_request.getPath() + "\r\n");
				this->_response.append("Accept-Ranges: bytes\r\n"); //Can be none but useless, only bytes ranges is defined by RFC
			}
		}

		void _buildBody(){
			this->_response.append("\r\n");
			this->_response.append(this->_content + "\r\n");
		}

		int			_statusIsCustom(int status){
			std::vector<std::string> vecError = this->_request.getErrorPage().first;
			size_t i;
			for (i = 0; i < vecError.size(); i++){
				char *charConfig = new char[(vecError.at(i) + ".html").size()];
				char *charStatus = new char[(itos(status) + ".html").size()];
				strcpy(charStatus, (itos(status) + ".html").c_str());
				strcpy(charConfig, (vecError.at(i) + ".html").c_str());
				if (match(charConfig, charStatus) == true){
					delete [] charConfig;
					delete [] charStatus;
					return (i);
				}
				delete [] charConfig;
				delete [] charStatus;
			}
			return (-1);
		}

		std::string	_buildErrorPage(int status){
			std::string ret;

			if (this->_request.getErrorPage().second == true && this->_statusIsCustom(status) != -1){
				std::string errorPagesPath = this->_request.getRootPath() + this->_request.getErrorPage().first.at( this->_request.getErrorPage().first.size() - 1);

				DIR *rep = NULL;
				struct dirent *fileRead = NULL;
				rep = opendir(errorPagesPath.c_str());
				if (rep == NULL){
					this->_status = INTERNAL_SERVER_ERROR;
					return (this->_buildErrorPage(INTERNAL_SERVER_ERROR));
				}
				else{
					char *toCompare = new char[(itos(status) + ".html").size()];
					strcpy(toCompare, (itos(status) + ".html").c_str());

					while ((fileRead = readdir(rep)) != NULL){
						if (match(toCompare, fileRead->d_name) == true){
							std::ifstream indata((this->_request.getRootPath().c_str() + this->_request.getErrorPage().first.at(1) + fileRead->d_name).c_str());
							std::stringstream buff;
							buff << indata.rdbuf();
							ret.append(buff.str());

							delete[] toCompare;
							if (closedir(rep) == -1){
								this->_status = INTERNAL_SERVER_ERROR;
								return (this->_buildErrorPage(INTERNAL_SERVER_ERROR));
							}
							return (ret);
						}
					}
					delete[] toCompare;
					if (closedir(rep) == -1){
						this->_status = INTERNAL_SERVER_ERROR;
						return (this->_buildErrorPage(INTERNAL_SERVER_ERROR));
					}
					ret.append(buildErrorPage(status));
				}
			}
			else
				ret.append(buildErrorPage(status));

			return (ret);
		}

		bool	_contentNeedRefresh(){
			//Si le ETag de la ressource correspond au champs If-None-Match on renvoie 304 et pas de content (Prioritaire sur If-Modified-Since)
			if (this->_request.findHeader("If-None-Match").empty() == false){
				if (this->_request.findHeader("If-None-Match") == makeETag(this->_pathToRoot)){
				//if (this->_request.findHeader("If-None-Match") == makeETag(this->_request.getRootPath() + this->_request.getPath() + "\r\n")){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			//Si la date de modification de la ressource correspond au champs If-Modified-Since on renvoie 304 et pas de content
			if (this->_request.findHeader("If-Modified-Since").empty() == false){
				if (this->_request.findHeader("If-Modified-Since") == getFileModification(this->_pathToRoot)){
				//if (this->_request.findHeader("If-Modified-Since") == getFileModification(this->_request.getRootPath() + this->_request.getPath() + "\r\n")){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			return (true);
		}

		//Retrieve a ressource
		void	_retrieveContent(){
			//If there is an error
			if (this->_status / 100 == 4 || this->_status / 100 == 5){
				this->_contentType = "text/html";
				this->_content.append(this->_buildErrorPage(this->_status));
				return;
			}
			//If there is no error but need no content
			else if (this->_status / 100 == 2 && this->_request.getMethod() != "GET")
				return;

			//GET method
			//else if (this->_request.getMethod() == "GET")
			if (this->_request.getMethod() == "GET"){
				//this->_get(this->_pathToRoot);
				std::string path = this->_request.getRootPath();
				this->_get(path);
			}
		}

		void	_get(std::string &path){
			if (this->_request.getPath() == "/"){
				this->_contentType = "text/html";
				if (this->_request.getAutoindex() == true)
					this->_content.append(buildAutoIndex(this->_request.getRootPath(), this->_request.getIndex()));
				else{
					path.append(this->_request.getIndex()[0]);
					std::ifstream indata(path.c_str());
					std::stringstream buff;
					buff << indata.rdbuf();
					this->_content.append(buff.str());
				}
			}
			else{
				this->_contentType = getMimeTypes(path.c_str());
				std::ifstream indata(path.c_str());
				std::stringstream buff;
				buff << indata.rdbuf();
				this->_content.append(buff.str());
			}
		}

		//Upload a ressource
		void	_uploadContent(){
			return;
		}

		//Delete a ressource
		void	_delete(){
			//if (remove(this->_pathToRoot.c_str()) != 0){
			//	this->_status = METHOD_NOT_ALLOWED;
			//	return;
			//}
			return;
		}

		//Request
		httpRequest						_request;

		//Response
		int								_status;
		std::string						_content;
		std::string						_contentType;
		std::string						_pathToRoot;

		std::string						_response;
};//end of class httpResponse

#endif