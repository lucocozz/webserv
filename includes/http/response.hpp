/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:52 by user42            #+#    #+#             */
/*   Updated: 2022/03/12 02:00:57 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include "../socket/Epoll.hpp"

#include "request.hpp"
#include "headersFunctions.hpp"
#include "mimeTypes.hpp"

#include <fstream>
#include <cstdio>

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

		void	buildResponse(httpRequest const &request){
			//Retrieves header info
			this->_request = request;
			this->_status = request.getStatus();

			//need to locat where i am from the server root to adapt the path
			if (_pathToRoot.empty() == false)
				_pathToRoot.clear();
			_pathToRoot = "../../../../..";
			_pathToRoot.append(this->_request.getPath());

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
			std::cout << _response << std::endl;
		}

	private:

		/*
			Build the response:
			1. Build the status line (PROTOCOL CODE MESSAGE)
			2. Build appropriate header (depending of the method used)
			3. Mount the content retrieved in _content
		*/
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
				std::cout << "IF I REMOVE THIS OUTPUT IT CRASH" << std::endl;
				this->_response.append("Last-Modified: " + getFileModification(this->_pathToRoot) + "\r\n");
				this->_response.append("Etag: " + makeETag(this->_pathToRoot) + "\r\n");
				this->_response.append("Accept-Ranges: bytes\r\n"); //Can be none but useless, only bytes ranges is defined by RFC
			}
		}

		void _buildBody(){
			this->_response.append("\r\n");
			this->_response.append(this->_content + "\r\n");
		}

		bool	_contentNeedRefresh(){
			//Si le ETag de la ressource correspond au champs If-None-Match on renvoie 304 et pas de content (Prioritaire sur If-Modified-Since)
			if (this->_request.findHeader("If-None-Match").empty() == false){
				if (this->_request.findHeader("If-None-Match") == makeETag(this->_pathToRoot)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			//Si la date de modification de la ressource correspond au champs If-Modified-Since on renvoie 304 et pas de content
			if (this->_request.findHeader("If-Modified-Since").empty() == false){
				if (this->_request.findHeader("If-Modified-Since") == getFileModification(this->_pathToRoot)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			return (true);
		}

		/*
			1. Retrieve a ressource
			1.1 GET
			2. Upload a ressource
			3. DELETE
		*/

		//Retrieve a ressource
		void	_retrieveContent(){
			//If there is an error
			if (this->_status / 100 == 4 || this->_status / 100 == 5){
				this->_contentType = "text/html";
				this->_content.append(buildErrorPage(this->_status));
				return;
			}
			//If there is no error but need no content
			else if (this->_status / 100 == 2 && this->_request.getMethod() != "GET")
				return;

			//Temporary
			if (this->_request.getMethod() == "GET" && this->_request.getPath() == "/"){
				this->_content.append("<html>\n<head><title>Welcome to 42webserv</title></head>\n");
				this->_content.append("<center><h3>" + itos(this->_status) + " " + getStatusMessage(this->_status) + "</h3></center>");
				this->_content.append("<body>\n<center><h1>Welcome to 42webserv !</h1></center>\n");
				this->_content.append("<p><center>If you see this page, the 42webserv is successfully installed and working. Further configuration is required.</center></p>\n");
				this->_content.append("<hr><center>42webserv/0.0.1</center>\n");
				this->_content.append("</body>\n</html>");
				this->_contentType = getMimeTypes("text/html");
			}
			//GET method
			else if (this->_request.getMethod() == "GET")
				this->_get(this->_pathToRoot);
		}

		void	_get(std::string &path){
			this->_contentType = getMimeTypes(path.c_str());
			
			std::ifstream indata(path.c_str());
			std::stringstream buff;
			buff << indata.rdbuf();
			this->_content.append(buff.str());
			//this->_content.append(path.c_str());
		}

		//Upload a ressource
		void	_uploadContent(){
			return;
		}

		//Delete a ressource
		void	_delete(){
			if (remove(this->_pathToRoot.c_str()) != 0){
				this->_status = METHOD_NOT_ALLOWED;
				return;
			}
			return;
		}

		//Members
		httpRequest						_request;

		int								_status;
		std::string						_content;
		std::string						_contentType;
		std::string						_pathToRoot;

		std::string						_response;
};//end of class httpResponse

#endif