/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:52 by user42            #+#    #+#             */
/*   Updated: 2022/03/25 02:21:08 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

/*
	Includes :
*/

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
#include "../CGI/CGI.hpp"
/*
	httpResponse class :
*/

class httpResponse{
	public:

		/*
			Coplien form :
			-	Default constructor
			-	Copy constructor
			-	Destructor
			-	Assignement operator
		*/

		httpResponse(){
			return;
		}

		httpResponse(httpResponse const &src){
			*this = src;
			return;
		}

		~httpResponse(){
			return;
		}

		httpResponse	operator=(httpResponse const &src){
			this->_request = src.getRequest();
			this->_status = src.getStatus();
			this->_content = src.getContent();
			this->_contentType = src.getContentType();
			this->_rootToFile = src.getRootToFile();
			this->_response = src.getResponse();
			return (*this);
		}

		/*
			Public functions :
			- Build the response from the request
			- Send the response to the client
		*/

		void	buildResponse(httpRequest const &request, Config const &config, const std::pair<std::string, std::string> &clientInfo){
			(void)config;
			this->_request = request;
			this->_status = request.getStatus();

			//need to locat where i am from the server root to adapt the path
			this->_rootToFile = this->_request.getRootPath();
			this->_rootToFile.erase(this->_rootToFile.end() - 1);
			if (this->_request.getPath() == "/" && this->_request.getAutoindex() == true)
				this->_rootToFile.append(this->_request.getIndex()[0]);
			else
				this->_rootToFile.append(this->_request.getPath());

			if (this->_request.getMethod() == "POST")
				this->_uploadContent();
			else if (this->_request.getMethod() == "DELETE")
				this->_delete();
			this->_retrieveContent(config, clientInfo, request.getHeaders());

			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();

			std::cout << std::endl << "SERVER RESPONSE :" << std::endl;
			std::cout << this->_response << std::endl;
		}

		void	sendResponse(EpollSocket &socketEvent){
			socketEvent.sendData(this->_response);
		}

		/*
			Getters :
		*/

		httpRequest						getRequest() const{return (this->_request);}

		int								getStatus() const{return (this->_status);}

		std::string						getContent() const{return (this->_content);}

		std::string						getContentType() const{return (this->_contentType);}

		std::string						getRootToFile() const{return (this->_rootToFile);}

		std::string						getResponse() const{return (this->_response);}

	/*
		Underluying functions :
	*/

	private:

		/*
			Build the response :
			-	Build the Status line (Protocol + Status_Code + Status_Message)
			-	Build the headers (Informations about the request)
			-	Build the body (An empty line and the body i retrieved if there is one)
		*/

		void _buildStatusLine(){
			this->_response.append("HTTP/1.1 ");
			this->_response.append(itos(this->_status) + " ");
			this->_response.append(getStatusMessage(this->_status) + "\r\n");
		}

		void _buildHeaders(){
			//Mandatory HEADER (even if error)
			this->_response.append("Server: " + this->_request.getServerName() + "\r\n");
			this->_response.append("Date: " + getActualTime() + "\r\n");
			this->_response.append("Content-Type: " + this->_contentType + "\r\n");
			this->_response.append("Content-Length: " + itos(this->_content.size()) + "\r\n");
			this->_response.append("Connection: keep-alive\r\n"); //close or keep-alive make an enum
			if (this->_status / 100 == 2 || this->_status / 100 == 3){
				if (this->_request.getAutoindex() == false){
					this->_response.append("Last-Modified: " + getFileModification(this->_rootToFile) + "\r\n");
					this->_response.append("Etag: " + makeETag(this->_rootToFile) + "\r\n");
					//Transfert-Enconding (Need to handle chunked request)
				}
				this->_response.append("Accept-Ranges: bytes\r\n");
			}
		}


		void _buildBody(){
			this->_response.append("\r\n");
			this->_response.append(this->_content + "\r\n");
		}

		/*
			Error pages :
			- Build and return an error page (default or custom)
			- Function to identify if the status code need a custom error page
		*/

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

		/*
			(COM) : Need to merge and clean retrieveContent / GET method
			Retrieve a ressource :
			- Retrieve the content
			- Function to identify if the client need to refresh the content (ETag / Last-Modified)
			- GET Method
		*/

		void	_retrieveContent(const Config &serverConfig, const std::pair<std::string, std::string> &clientInfo, const std::map<std::string, std::string> &headers){
			if (this->_status / 100 == 4 || this->_status / 100 == 5){
				this->_contentType = "text/html";
				this->_content.append(this->_buildErrorPage(this->_status));
				return;
			}
			else if (this->_status / 100 == 2 && this->_request.getMethod() != "GET")
				return;

			//GET method
			if (this->_request.getMethod() == "GET"){
				std::string path = this->_request.getRootPath();
				this->_get(path, serverConfig, clientInfo, headers);
			}
		}

		bool	_contentNeedRefresh(){
			//Si le ETag de la ressource correspond au champs If-None-Match on renvoie 304 et pas de content (Prioritaire sur If-Modified-Since)
			if (this->_request.findHeader("If-None-Match").empty() == false){
				if (this->_request.findHeader("If-None-Match") == makeETag(this->_rootToFile)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			//Si la date de modification de la ressource correspond au champs If-Modified-Since on renvoie 304 et pas de content
			if (this->_request.findHeader("If-Modified-Since").empty() == false){
				if (this->_request.findHeader("If-Modified-Since") == getFileModification(this->_rootToFile)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			return (true);
		}

		static std::pair<bool,LocationContext>  getLocation(std::string path, std::vector<LocationContext> serverLocation){
		std::pair<bool,LocationContext> locationPair = std::make_pair(true, serverLocation[0]);

		for (size_t i = 0; i < serverLocation.size(); i++){
			if ((path.append("/").find(serverLocation[i].args[0]) != std::string::npos) && 
				(serverLocation[i].directives.count("cgi_binary") == 1)){
				locationPair.second = serverLocation[i];
				return(locationPair);
			}
		}
		std::cout << "pas loca " << serverLocation[0].args[0] << std::endl;
		locationPair.first = false;
		return (locationPair);
		}

		void	_get(std::string &path, const Config &serverConfig, const std::pair<std::string, std::string> &clientInfo, const std::map<std::string, std::string> &headers){
			std::pair<bool,LocationContext> locationResult = 
				getLocation(this->_request.getPath(), serverConfig.servers[0].locations);
			if (locationResult.first == true){
				try{
					std::pair<std::string, int> cgiResponse;
					//serverConfig.servers[0] ->  will change depending on what server we work on
					std::pair<ServerContext, LocationContext > serverLocation = 
						std::make_pair(serverConfig.servers[0], locationResult.second);

					CGI cgi(this->_request.getPath(), headers, serverLocation, clientInfo, "GET");
					cgiResponse = cgi.cgiHandler();
					this->_content.append(cgiResponse.first);
					this->_contentType = "text/html";
					this->_status = cgiResponse.second;
				}
				catch(const std::exception &e){
					this->_status = 500;
					std::cout << "Cgi failed: " << e.what() << std::endl;
				}
			}
			else if (this->_request.getPath() == "/"){
				this->_contentType = "text/html";
				if (this->_request.getAutoindex() == true)
					this->_content.append(buildAutoIndex(this->_request.getRootPath(), this->_request.getIndex()));
				else{
					std::ifstream indata(path.c_str());
					std::stringstream buff;
					buff << indata.rdbuf();
					this->_content.append(buff.str());
				}
			}
			else{
				path.erase(path.end() - 1);
				path.append(this->_request.getPath());
				this->_contentType = getMimeTypes(path.c_str());
				std::ifstream indata(path.c_str());
				std::stringstream buff;
				buff << indata.rdbuf();
				this->_content.append(buff.str());
			}
		}

		/*
			Upload a ressource :
		*/

		void	_uploadContent(){
			return;
		}

		/*
			Delete a ressource :
		*/

		void	_delete(){
			return;
		}

		/*
			Variables :
		*/

		httpRequest						_request;

		int								_status;
		std::string						_content;
		std::string						_contentType;
		std::string						_rootToFile;
		std::string						_response;
};//end of class httpResponse

#endif