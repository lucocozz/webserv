/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:52 by user42            #+#    #+#             */
/*   Updated: 2022/04/18 18:01:49 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP


/*
	Includes :
*/


#include "request.hpp"
#include "CGI.hpp"

//#include "EpollSocket.hpp"
//#include "statusCode.hpp"
//#include "locationRelated.hpp"
//#include "URLRelated.hpp"




#include <fstream>

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
			this->_statusMessages = initStatusMessages();
			this->_extensionTypes = initExtensionTypes();
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

		void	buildResponse(httpRequest const &request, Server const &server, const std::pair<std::string, std::string> &clientInfo){
			this->_statusMessages = initStatusMessages();
			this->_extensionTypes = initExtensionTypes();
			this->_request = request;
			this->_status = request.getStatus();
			this->_rootToFile = buildPathTo(this->_request.getRootPath(), this->_request.getPath(), "");

			if (this->_request.getMethod() == "POST" && isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true)
				this->_uploadContent(request.getPath(), server, clientInfo, request.getHeaders());
			else if (this->_request.getMethod() == "DELETE" && isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true)
				this->_deleteContent();
			this->_retrieveContent(server, clientInfo);

			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();

			//DEBUG OUTPUT
			std::cout << std::endl << "SERVER RESPONSE :" << std::endl;
			std::cout << this->_response << std::endl;
		}

		void	sendResponse(EpollSocket socketEvent){
			socketEvent.sendData(this->_response);
			this->clear();
		}

		void					clear(){
			this->_request.clear();

			this->_status = 200;
			this->_statusMessages.clear();
			this->_extensionTypes.clear();
			this->_content.clear();
			this->_contentType.clear();
			this->_rootToFile.clear();
			this->_response.clear();
		}

		/*
			Getters :
		*/

		httpRequest						getRequest() const{return (this->_request);}

		int								getStatus() const{return (this->_status);}

		std::string						getStatusMessage() const{return ((*this->_statusMessages.find(this->_status)).second);}

		std::string						getContent() const{return (this->_content);}

		std::string						getContentType() const{return (this->_contentType);}

		std::string						getRootToFile() const{return (this->_rootToFile);}

		std::string						getResponse() const{return (this->_response);}

		std::string						getMimeTypes(char const *path) const{
			std::string tmp(path);
			if (tmp.rfind(".") == std::string::npos)
				return ("text/html");
			std::map<std::string, std::string>::const_iterator it = this->_extensionTypes.find(tmp.substr(tmp.rfind(".") + 1));
			if (it == this->_extensionTypes.end())
				return ("text/html");
			return ((*it).second);
		}

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
			this->_response.append(this->getStatusMessage() + "\r\n");
		}

		void _buildHeaders(){
			this->_response.append("Server: " + this->_request.getServerName() + "\r\n");
			this->_response.append("Date: " + buildDate() + "\r\n");
			this->_response.append("Content-Type: " + this->_contentType + "\r\n");
			this->_response.append("Content-Length: " + itos(this->_content.size()) + "\r\n");
			this->_response.append("Connection: keep-alive\r\n");
			/*if (this->_status / 100 == 2 || this->_status / 100 == 3){
				if (this->_request.getAutoindex() == false){
					if (this->_request.getPath() == "/" && this->_request.getIndex().empty() == false){
						std::string pathToIndex = buildPathTo(this->_request.getRootPath(), this->_request.getIndex(), "");
						this->_response.append("Last-Modified: " + formatLastModified(pathToIndex) + "\r\n");
						this->_response.append("Etag: " + formatETag(pathToIndex) + "\r\n");
					}
					else if (isPathDirectory(this->_rootToFile.c_str()) == false){
						this->_response.append("Last-Modified: " + formatLastModified(this->_rootToFile) + "\r\n");
						this->_response.append("Etag: " + formatETag(this->_rootToFile) + "\r\n");
					}
					else{
						std::pair<std::string, std::string> indexLocation = retrieveLocationIndex(this->_request.getLocations(), this->_request.getRootPath(), this->_request.getPath());
						if (indexLocation.first.empty() == false && isSameDirectory(indexLocation.second, this->_request.getPath()) == true){
							std::string pathToLocationIndex = buildPathTo(this->_request.getRootPath(), indexLocation.first, "");
							this->_response.append("Last-Modified: " + formatLastModified(pathToLocationIndex) + "\r\n");
							this->_response.append("Etag: " + formatETag(pathToLocationIndex) + "\r\n");
						}
					}
				}
			}*/
		}

		void _buildBody(){
			this->_response.append("\r\n");
			this->_response.append(this->_content + "\r\n");
		}

		/*
			Retrieve a ressource :
			- Retrieve the content
			- Identify if the client need to refresh the content (ETag / Last-Modified)
			- Build the autoindex page
		*/

		void	_retrieveContent(const Server &server, const std::pair<std::string, std::string> &clientInfo){
			if (this->_request.getMethod() == "GET" && isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true){
				//SEGFAULT
				(void)server;(void)clientInfo;
				std::pair<bool,LocationContext> locationResult = getLocation(this->_request.getPath(), server.context.locations);
				std::cout << "DEBUG ?" << std::endl;
				if (locationResult.first == true){
					std::cout << "DEBUG je veux acceder aux CGI" << std::endl;
					this->_retrieveCGIContent(server, clientInfo, locationResult.second);
				}
				else if (this->_request.getPath() == "/"/* && _contentNeedRefresh() == true*/){
					this->_contentType = "text/html";
					if (this->_request.getAutoindex() == true)
						this->_buildAutoIndex(this->_request.getRootPath(), this->_request.getPath());
					else
						_retrieveFileContent(buildPathTo(this->_rootToFile, this->_request.getIndex(), ""));
				}
				else/* if (_contentNeedRefresh() == true)*/{
					if (isPathValid(this->_rootToFile) == false){
						this->_buildErrorPage(NOT_FOUND);
						return;
					}
					std::pair<std::string, std::string> indexLocation = retrieveLocationIndex(this->_request.getLocations(), this->_request.getRootPath(), this->_request.getPath());
					if (indexLocation.first.empty() == false && isSameDirectory(indexLocation.second, this->_request.getPath()) == true)
						_retrieveFileContent(buildPathTo(this->_request.getRootPath(), indexLocation.first, ""));
					else if (this->_request.getAutoindex() == true && isPathDirectory(this->_rootToFile) == true)
						this->_buildAutoIndex(this->_request.getRootPath(), this->_request.getPath());
					else
						_retrieveFileContent(this->_rootToFile);
				}
			}
			else if (this->_request.getMethod() == "GET" || isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == false)
				this->_status = METHOD_NOT_ALLOWED;

			if (this->_status / 100 == 4 || this->_status / 100 == 5){
				this->_contentType = "text/html";
				this->_buildErrorPage(this->_status);
				return;
			}
		}

		void			_retrieveCGIContent(Server const &server, const std::pair<std::string, std::string> &clientInfo, LocationContext context){
			try{
				std::pair<std::string, int> cgiResponse;
				std::pair<ServerContext, LocationContext > serverLocation = 
					std::make_pair(server.context, context);
				CGI cgi(this->_request.getPath(), this->_request.getHeaders(), this->_request.getBody(), serverLocation, clientInfo, "GET");
				cgiResponse = cgi.cgiHandler();
				this->_content.append(cgiResponse.first);
				this->_contentType = "text/html";
				this->_status = cgiResponse.second;
			}
			catch(const std::exception &e){
				std::string exception(e.what());
				if (exception.find("No such file or directory") != std::string::npos){
					this->_status = 404;
					return;
				}
				this->_status = 500;
				std::cerr << "Cgi failed: " << exception << std::endl;
			}
		}

		void			_retrieveFileContent(std::string pathToIndex){
			this->_contentType = this->getMimeTypes(pathToIndex.c_str());
			std::ifstream indata(pathToIndex.c_str());
			std::stringstream buff;
			buff << indata.rdbuf();
			this->_content.append(buff.str());
		}

		/*bool	_contentNeedRefresh(){
			//Si le ETag de la ressource correspond au champs If-None-Match on renvoie 304 et pas de content (Prioritaire sur If-Modified-Since)
			if (this->_request.findHeader("If-None-Match").empty() == false){
				if (this->_request.findHeader("If-None-Match") == formatETag(this->_rootToFile)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			//Si la date de modification de la ressource correspond au champs If-Modified-Since on renvoie 304 et pas de content
			if (this->_request.findHeader("If-Modified-Since").empty() == false){
				if (this->_request.findHeader("If-Modified-Since") == formatLastModified(this->_rootToFile)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			return (true);
		}*/

		void			_buildAutoIndex(std::string rootPath, std::string path){
			this->_content.append("<html>\r\n<head>\r\n");
			this->_content.append("<title>Index of " + path + "</title>\r\n</head>\r\n<body>\r\n");
			this->_content.append("<h1>Index of " + path + "</h1>\r\n<hr>\r\n");
			DIR *rep = NULL;
			struct dirent *fileRead = NULL;
			rep = opendir(buildPathTo(rootPath, path, "").c_str());
			if (rep == NULL){
				this->_buildErrorPage(INTERNAL_SERVER_ERROR);
				return ;
			}
			else{
				this->_content.append(buildListingBlock(fileRead, rep, rootPath, path, this->_request.findHeader("Host")));
				if (closedir(rep) == -1){
					this->_buildErrorPage(INTERNAL_SERVER_ERROR);
					return;
				}
			}
			this->_content.append("<hr>\r\n");
			this->_content.append("<form enctype=\"multipart/form-data\" action=\"/php-cgi/upload.php\" method=\"post\">");
  			this->_content.append("Envoyer ce fichier : <input name=\"userfile\" type=\"file\" />");
 			this->_content.append("<input type=\"submit\" value=\"Envoyer le fichier\" />");
			this->_content.append("</form>");
			//POST form 1 input
			this->_content.append("<hr>\r\n");
			this->_content.append("Multipart Form POST 1 input : \r\n");
			this->_content.append("<form enctype=\"multipart/form-data\" action=\"/\" method=\"post\">");
			this->_content.append("<input type=\"file\" name=\"monFichier\" />");
 			this->_content.append("<button type =\"submit\">Envoyer</button>");
			this->_content.append("</form>");
			this->_content.append("</body>\r\n</html>\r\n");
		}

		/*
			Upload a ressource :
		*/

		void	_uploadContent(const std::string &path, const Server &server, const std::pair<std::string, std::string> &clientInfo, const std::map<std::string, std::string> &headers){
			std::pair<bool,LocationContext> locationResult = 
				getLocation(this->_request.getPath(), server.context.locations);
			if (locationResult.first == true){
				try{
					std::pair<std::string, int> cgiResponse;
					std::pair<ServerContext, LocationContext > serverLocation = 
						std::make_pair(server.context, locationResult.second);

					CGI cgi(path, headers, this->_request.getBody() , serverLocation, clientInfo, "POST");
					cgiResponse = cgi.cgiHandler();
					this->_content.append(cgiResponse.first);
					this->_contentType = "text/html";
					this->_status = cgiResponse.second;
				}
				catch(const std::exception &e){
					this->_status = 500;
					std::cerr << "Cgi failed: " << e.what() << std::endl;
				}
			}
			//MULTIPART
			else if (this->_request.getBoundarie().first == true){
				for (std::map<std::map<std::string, std::string>, std::string>::const_iterator it = this->_request.getBodyMultipart().begin(); it != this->_request.getBodyMultipart().end(); it++){
					std::string pathToFile = buildPathTo(this->_request.getRootPath(), this->_request.getPath(), (*(*it).first.find("filename")).second);
					this->_contentType = this->getMimeTypes(pathToFile.c_str());
					std::ofstream outdata(pathToFile.c_str());
					outdata << (*it).second << std::endl;
					outdata.close();
				}
			}
			else{
				std::string pathToFile = buildPathTo(this->_request.getRootPath(), this->_request.getPath(), "request.txt");
				this->_contentType = this->getMimeTypes(pathToFile.c_str());
				std::ofstream outdata(pathToFile.c_str());
				outdata << this->_request.getBody() << std::endl;
				outdata.close();
			}
			return;
		}

		/*
			Delete a ressource :
		*/

		void	_deleteContent(){
			if (this->_request.getPath() == "/")
				this->_buildErrorPage(FORBIDDEN);
			else if (isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true){
				if (isPathValid(_rootToFile) && isPathDirectory(_rootToFile) == false)
					remove(_rootToFile.c_str());
				else if (isPathValid(_rootToFile) && isPathDirectory(_rootToFile) == true){
					if (removeDir(_rootToFile.c_str()) == -1)
						this->_buildErrorPage(FORBIDDEN);
				}
				else
					this->_buildErrorPage(NOT_FOUND);
			}
			else
				this->_buildErrorPage(METHOD_NOT_ALLOWED);
			return;
		}

		/*
			Error pages :
			- Build an error page (custom or default)
			- Build a default error page (generated)
		*/

		void					_buildErrorPage(int status){
			std::string ret;

			this->_content.clear();
			this->_status = status;
			if (this->_request.getErrorPage().second == true){
				std::map<std::string, std::string>::const_iterator it = this->_request.getErrorPage().first.begin();
				for (; it != this->_request.getErrorPage().first.end(); it++){
					if (match(itos(status).c_str(), (*it).first.c_str(), 'x') == 1)
						break;
				}
				if (it == this->_request.getErrorPage().first.end()){
					this->_buildDefaultErrorPage();
					return;
				}
				this->_retrieveFileContent(buildPathTo(this->_request.getRootPath(), (*it).second, ""));
				return;
			}
			this->_buildDefaultErrorPage();
		}

		void					_buildDefaultErrorPage(){
			std::string title = itos(this->_status) + " " + this->getStatusMessage();
			this->_contentType = "text/html";
			this->_content.clear();
			this->_content.append("<html>\n<head><title>" + title + "</title></head>\n");
			this->_content.append("<body>\n<center><h1>" + title + "</h1></center>\n");
			this->_content.append("<hr><center>42webserv/0.0.1</center>\n");
			this->_content.append("</body>\n</html>");
		}

		/*
			Variables :
		*/

		httpRequest							_request;

		int									_status;
		std::map<int, std::string>			_statusMessages;
		std::map<std::string, std::string>	_extensionTypes;
		std::string							_content;
		std::string							_contentType;
		std::string							_rootToFile;
		std::string							_response;
};//end of class httpResponse

#endif
