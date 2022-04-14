/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:52 by user42            #+#    #+#             */
/*   Updated: 2022/04/14 18:14:54 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP

/*
	Includes :
*/

#include "Config.hpp"
#include "Epoll.hpp"
#include "request.hpp"
#include "mimeTypes.hpp"
#include "Server.hpp"
#include "CGI.hpp"

#include <fstream>
#include <cstdio>
#include <dirent.h>
#include <sys/types.h>

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
			this->_statusMessages = initStatusMessages();
			this->_extensionTypes = initExtensionTypes();
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
			this->_request = request;
			this->_status = request.getStatus();

			//buildThePath
			this->_rootToFile = buildPathTo(this->_request.getRootPath(), this->_request.getPath(), "");

			//Treatment
			if (this->_request.getMethod() == "POST" && isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true)
				this->_uploadContent(request.getPath(), server, clientInfo, request.getHeaders());
			else if (this->_request.getMethod() == "DELETE" && isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true)
				this->_deleteContent();
			this->_retrieveContent(server, clientInfo, request.getHeaders());
			//Build response
			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();

			std::cout << std::endl << "SERVER RESPONSE :" << std::endl;
			std::cout << this->_response << std::endl;
		}

		void	sendResponse(EpollSocket socketEvent){
			socketEvent.sendData(this->_response);
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
			//Mandatory HEADER (even if error)
			this->_response.append("Server: " + this->_request.getServerName() + "\r\n");
			this->_response.append("Date: " + buildDate() + "\r\n");
			this->_response.append("Content-Type: " + this->_contentType + "\r\n");
			this->_response.append("Content-Length: " + itos(this->_content.size()) + "\r\n");
			this->_response.append("Connection: keep-alive\r\n");
			//if (this->_status / 100 == 2 || this->_status / 100 == 3){
			//	if (this->_request.getAutoindex() == false){
			//		this->_response.append("Last-Modified: " + buildLastModified(this->_rootToFile) + "\r\n");
			//		this->_response.append("Etag: " + buildETag(this->_rootToFile) + "\r\n");
			//	}
			//}
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

		std::string					_buildDefaultErrorPage(){
			std::string ret;
			std::string title = itos(this->_status) + " " + this->getStatusMessage();
			ret.append("<html>\n<head><title>" + title + "</title></head>\n");
			ret.append("<body>\n<center><h1>" + title + "</h1></center>\n");
			ret.append("<hr><center>42webserv/0.0.1</center>\n");
			ret.append("</body>\n</html>");

			return (ret);
		}

		void	_buildErrorPage(int status){
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
					this->_content.append(this->_buildDefaultErrorPage());
					return;
				}
				std::ifstream indata(buildPathTo(this->_request.getRootPath(), (*it).second, "").c_str());
				std::stringstream buff;
				buff << indata.rdbuf();
				this->_content.append(buff.str());
				return;
			}
			this->_content.append(this->_buildDefaultErrorPage());
		}

		/*
			(COM) : Need to merge and clean retrieveContent / GET method
			Retrieve a ressource :
			- Retrieve the content
			- Function to identify if the client need to refresh the content (ETag / Last-Modified)
		*/

		bool	_contentNeedRefresh(){
			//Si le ETag de la ressource correspond au champs If-None-Match on renvoie 304 et pas de content (Prioritaire sur If-Modified-Since)
			if (this->_request.findHeader("If-None-Match").empty() == false){
				if (this->_request.findHeader("If-None-Match") == buildETag(this->_rootToFile)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			//Si la date de modification de la ressource correspond au champs If-Modified-Since on renvoie 304 et pas de content
			if (this->_request.findHeader("If-Modified-Since").empty() == false){
				if (this->_request.findHeader("If-Modified-Since") == buildLastModified(this->_rootToFile)){
					this->_status = NOT_MODIFIED;
					return (false);
				}
			}
			return (true);
		}

		void	_retrieveContent(const Server &server, const std::pair<std::string, std::string> &clientInfo, const std::map<std::string, std::string> &headers){
			if (this->_request.getMethod() == "GET" && isMethodAllowed(this->_request.getLocations(), this->_request.getPath(), this->_request.getMethod()) == true){
				std::pair<bool,LocationContext> locationResult = getLocation(this->_request.getPath(), server.context.locations);
				if (locationResult.first == true){
					try{
						std::pair<std::string, int> cgiResponse;
						std::pair<ServerContext, LocationContext > serverLocation = 
							std::make_pair(server.context, locationResult.second);
						CGI cgi(this->_request.getPath(), headers, this->_request.getBody(), serverLocation, clientInfo, "GET");
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
				//If request path is root
				//else if (this->_request.getPath() == "/" && _contentNeedRefresh() == true){
				else if (this->_request.getPath() == "/"){
					this->_contentType = "text/html";
					//Listing the directories from the root
					if (this->_request.getAutoindex() == true)
						this->_buildAutoIndex(this->_request.getRootPath(), this->_request.getPath());
					//Get the index page
					else{
						std::string indexPath = _rootToFile + this->_request.getIndex();
						std::ifstream indata(indexPath.c_str());
						std::stringstream buff;
						buff << indata.rdbuf();
						this->_content.append(buff.str());
					}
				}
				//else if (_contentNeedRefresh() == true){
				else{
					if (isPathValid(this->_rootToFile) == false){
						this->_buildErrorPage(NOT_FOUND);
						return;
					}
					std::pair<std::string, std::string> indexLocation = retrieveLocationIndex(this->_request.getLocations(), this->_request.getRootPath(), this->_request.getPath());
					if (indexLocation.first.empty() == false && isSameDirectory(indexLocation.second, this->_request.getPath()) == true){
						std::string pathToIndex = buildPathTo(this->_request.getRootPath(), indexLocation.first, "");
						this->_contentType = this->getMimeTypes(pathToIndex.c_str());
						
						std::ifstream indata(pathToIndex.c_str());
						std::stringstream buff;
						buff << indata.rdbuf();
						this->_content.append(buff.str());
					}
					else if (this->_request.getAutoindex() == true && isPathDirectory(this->_rootToFile) == true)
						this->_buildAutoIndex(this->_request.getRootPath(), this->_request.getPath());
					else{
						this->_contentType = this->getMimeTypes(this->_rootToFile.c_str());
						std::ifstream indata(this->_rootToFile.c_str());
						std::stringstream buff;
						buff << indata.rdbuf();
						this->_content.append(buff.str());
					}
				}
			}
			else if (this->_request.getMethod() == "GET")
				this->_status = METHOD_NOT_ALLOWED;
			if (this->_status / 100 == 4 || this->_status / 100 == 5){
				this->_contentType = "text/html";
				this->_buildErrorPage(this->_status);
				return;
			}
		}

		static std::pair<bool,LocationContext>  getLocation(std::string path,const std::vector<LocationContext> &serverLocation){
			LocationContext init;

			std::pair<bool,LocationContext> locationPair = std::make_pair(true, init);
			for (size_t i = 0; i < serverLocation.size(); i++){
				if ((path.append("/").find(serverLocation[i].args[0]) != std::string::npos) && 
					(serverLocation[i].directives.count("cgi_binary") == 1) &&
					(serverLocation[i].directives.count("cgi_extension") == 1) &&
					(path.find(serverLocation[i].directives.find("cgi_extension")->second[0])) != std::string::npos){
					locationPair.second = serverLocation[i];
					return(locationPair);
				}
			}
			locationPair.first = false;
			return (locationPair);
		}

		void			_buildAutoIndex(std::string rootPath, std::string path){
			this->_content.append("<html>\r\n<head>\r\n");
			this->_content.append("<title>Index of " + path + "</title>\r\n</head>\r\n");
			this->_content.append("<body>\r\n");
			this->_content.append("<h1>Index of " + path + "</h1>\r\n");
			this->_content.append("<hr>\r\n");
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
			else{
				std::string pathToIndex = buildPathTo(this->_request.getRootPath(), this->_request.getPath(), "request.txt");
				this->_contentType = this->getMimeTypes(pathToIndex.c_str());
				std::ofstream outdata(pathToIndex.c_str());
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
