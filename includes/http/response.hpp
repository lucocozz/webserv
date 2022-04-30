/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:52 by user42            #+#    #+#             */
/*   Updated: 2022/04/30 12:59:30 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
#define RESPONSE_HPP


/*
	Includes :
*/


#include "request.hpp"
#include "CGI.hpp"

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
			this->_locationRootPath = src.getLocationRootPath();
			this->_redirectionHeader = src.getRedirectionHeader();
			this->_allowedLocation = src.getAllowedLocation();

			this->_response = src.getResponse();
			return (*this);
		}

		/*
			Public functions :
			- Build the response from the request
			- Send the response to the client
		*/

		void	buildResponse(httpRequest *request, Server const &server, const std::pair<std::string, std::string> &clientInfo){
			this->_statusMessages = initStatusMessages();
			this->_extensionTypes = initExtensionTypes();
			this->_request = request;
			this->_status = request->getStatus();
			this->_rootToFile = buildPathTo(this->_request->getRootPath(), this->_request->getPath(), "");

			std::pair<std::string, std::string> locationRoot = retrieveLocationRoot(this->_request->getLocations(), this->_request->getRootPath(), this->_request->getPath());
			std::string oldPath = this->_request->getPath();
			if (locationRoot.first != this->_request->getRootPath()){
				this->_locationRootPath = locationRoot.first;
				this->_request->setPath(locationToRoot(this->_request->getPath(), locationRoot.first, locationRoot.second));
				this->_rootToFile = buildPathTo(this->_request->getRootPath(), this->_request->getPath(), "");
			}

			std::pair<int, std::string>	testRedirection = retrieveLocationRedirection(this->_request->getLocations(), oldPath);
			if ((testRedirection.first >= 301 && testRedirection.first <= 303) || testRedirection.first == 307){
				if (oldPath == testRedirection.second || buildUrlTo(this->_request->findHeader("Host"), oldPath, "") == testRedirection.second)
					this->_buildErrorPage(TOO_MANY_REDIRECTS, "");
				else{
					this->_redirectionHeader = testRedirection.second;
					this->_status = testRedirection.first;
				}
			}
			else if (testRedirection.first != 0){
				this->_buildErrorPage(testRedirection.first, testRedirection.second);
				return;
			}
			else{
				if (this->_request->getMethod() == "POST" && this->_isMethodAllowed(this->_request->getLocations(), oldPath, this->_request->getMethod(), this->_request->getAllowedMethod()) == true)
					this->_uploadContent(request->getPath(), server, clientInfo, request->getHeaders(), oldPath);
				else if (this->_request->getMethod() == "DELETE" && this->_isMethodAllowed(this->_request->getLocations(), oldPath, this->_request->getMethod(), this->_request->getAllowedMethod()) == true)
					this->_deleteContent(oldPath);
				this->_retrieveContent(server, clientInfo, oldPath);
			}

			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();
		}

		void	sendResponse(EpollSocket socketEvent){
			size_t 						header = 0;
			header = socketEvent.sendData(this->_response);
			std::string::iterator 	itb = this->_response.begin();
			std::string::iterator 	ite = itb + header;
			this->_response.erase(itb, ite);
		}

		void					clear(){
			this->_request->clear();

			this->_status = 200;
			this->_statusMessages.clear();
			this->_extensionTypes.clear();
			this->_content.clear();
			this->_contentType.clear();
			this->_rootToFile.clear();
			this->_locationRootPath.clear();
			this->_redirectionHeader.clear();
			this->_allowedLocation.clear();

			this->_response.clear();
		}

		/*
			Getters :
		*/

		httpRequest						*getRequest() const{return (this->_request);}

		const int						&getStatus() const{return (this->_status);}
		const std::string				&getStatusMessage() const{return ((*this->_statusMessages.find(this->_status)).second);}
		const std::string				&getContent() const{return (this->_content);}
		const std::string				&getContentType() const{return (this->_contentType);}
		const std::string				&getRootToFile() const{return (this->_rootToFile);}
		const std::string				&getLocationRootPath() const{return (this->_locationRootPath);}
		const std::string				&getRedirectionHeader() const{return (this->_redirectionHeader);}
		const std::vector<std::string>	&getAllowedLocation() const{return (this->_allowedLocation);}

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
			this->_response.append("Server: " + this->_request->getServerName() + "\r\n");
			this->_response.append("Date: " + buildDate() + "\r\n");
			if (this->_status / 100 == 3 && this->_status != 310 && this->_redirectionHeader.empty() == false)
				this->_response.append("Location: " + this->_redirectionHeader + "\r\n");
			if (this->_contentType.empty() == false)
				this->_response.append("Content-Type: " + this->_contentType + "\r\n");
			if (this->_status == METHOD_NOT_ALLOWED){
				this->_response.append("Allow:");
				if (this->_allowedLocation.empty() == true){
					for (size_t i = 0; i < this->_request->getAllowedMethod().size(); i++)
						this->_response.append(" " + this->_request->getAllowedMethod().at(i));
				}
				else{
					for (size_t i = 0; i < this->_allowedLocation.size(); i++)
						this->_response.append(" " + this->_allowedLocation.at(i));
				}
				this->_response.append("\r\n");
			}
			this->_response.append("Content-Length: " + itos(this->_content.size()) + "\r\n");
			this->_response.append("Transfer-Encoding: identity\r\n");
			this->_response.append("Connection: keep-alive\r\n");
		}

		void _buildBody(){
			this->_response.append("\r\n");
			this->_response.append(this->_content);
		}

		/*
			Retrieve a ressource :
			- Retrieve the content
			- Identify if the client need to refresh the content (ETag / Last-Modified)
			- Build the autoindex page
		*/

		void	_retrieveContent(const Server &server, const std::pair<std::string, std::string> &clientInfo, std::string oldPath){
			if (this->_request->getMethod() == "GET" && this->_isMethodAllowed(this->_request->getLocations(), this->_request->getPath(), this->_request->getMethod(), this->_request->getAllowedMethod()) == true){
				std::pair<bool,LocationContext> locationResult = cgiChecker(this->_request->getPath(), server.context.locations);
				if (locationResult.first == true)
					this->_retrieveCGIContent(server, clientInfo, locationResult.second);
				else if (this->_request->getPath() == "/"){
					if (this->_request->getAutoindex() == true)
						this->_buildAutoIndexPage(this->_request->getRootPath(), this->_request->getPath(), oldPath);
					else{
						if (this->_request->getIndex() == "default_index.html")
							this->_buildDefaultIndexPage();
						else
							this->_retrieveFileContent(buildPathTo(this->_rootToFile, this->_request->getIndex(), ""));
					}
				}
				else{
					std::pair<std::string, std::string> indexLocation = retrieveLocationIndex(this->_request->getLocations(), this->_request->getRootPath(), oldPath);
					if (isPathValid(this->_rootToFile) == false && retrieveLocationAutoIndex(this->_request->getLocations(), oldPath) == false && indexLocation.first.empty()){
						this->_buildErrorPage(NOT_FOUND, "");
						return;
					}
					std::pair<std::string, std::string> locationRoot = retrieveLocationRoot(this->_request->getLocations(), this->_request->getRootPath(), this->_request->getPath());
					if (retrieveLocationAutoIndex(this->_request->getLocations(), oldPath) == true){
						if (isPathValid(this->_rootToFile) == true && isPathDirectory(this->_rootToFile) == false)
							this->_retrieveFileContent(this->_rootToFile);
						else if (locationRoot.first.empty() == true && isPathValid(this->_rootToFile) == false)
							this->_buildAutoIndexPage(this->_request->getRootPath(), "/", oldPath);
						else
							this->_buildAutoIndexPage(this->_request->getRootPath(), this->_request->getPath(), oldPath);
					}
					else if (indexLocation.first.empty() == false && isSameDirectory(indexLocation.second, oldPath) == true){
						if (indexLocation.first == "default_index.html")
							this->_buildDefaultIndexPage();
						else
							this->_retrieveFileContent(buildPathTo(this->_request->getRootPath(), indexLocation.first, ""));
					}
					else if (isPathDirectory(this->_rootToFile) == true && (this->_request->getAutoindex() == true || pathIsLocation(this->_request->getPath(), this->_request->getLocations(), "autoindex").first == true))
						this->_buildAutoIndexPage(this->_request->getRootPath(), this->_request->getPath(), oldPath);
					else
						this->_retrieveFileContent(this->_rootToFile);
				}
			}
			else if (this->_isMethodAllowed(this->_request->getLocations(), oldPath, this->_request->getMethod(), this->_request->getAllowedMethod()) == false){
				this->_status = METHOD_NOT_ALLOWED;
			}
			if (this->_status / 100 == 4 || this->_status / 100 == 5){
				this->_contentType = "text/html";
				this->_buildErrorPage(this->_status, "");
				return;
			}
		}

		void			_retrieveCGIContent(Server const &server, const std::pair<std::string, std::string> &clientInfo, LocationContext context){
			try{
				std::pair<std::string, int> cgiResponse;
				std::pair<ServerContext, LocationContext > serverLocation = 
					std::make_pair(server.context, context);
				CGI cgi(this->_request->getPath(), this->_request->getHeaders(), this->_request->getBody(), serverLocation, clientInfo, "GET");
				cgiResponse = cgi.cgiHandler();
				this->_content.append(cgiResponse.first);
				this->_contentType = "text/html";
				this->_status = cgiResponse.second;
			}
			catch(const std::exception &e){
				std::string exception(e.what());
				if (exception.find("No such file or directory") != std::string::npos){
					this->_status = INTERNAL_SERVER_ERROR;
					return;
				}
				this->_status = INTERNAL_SERVER_ERROR;
				std::cerr << "Cgi failed: " << exception << std::endl;
			}
		}

		void			_retrieveFileContent(std::string pathToIndex){
			this->_contentType = this->getMimeTypes(pathToIndex.c_str());
			std::stringstream buff;
			if (this->_contentType.find("text/") != std::string::npos){
				std::ifstream indata(pathToIndex.c_str());
				buff << indata.rdbuf();
			}
			else{
				std::ifstream indata(pathToIndex.c_str(), std::ios::binary);
				buff << indata.rdbuf();
			}
			this->_content.append(buff.str());
		}

		void			_buildDefaultIndexPage(){
			this->_contentType = "text/html";
			this->_content.append("<!DOCTYPE html>\r\n<html>\r\n<head>\r\n<meta charset='utf-8'>\r\n");
			this->_content.append("<title>Welcome to " + this->_request->getServerName() + "</title>\r\n");
			this->_content.append("<style>\r\nbody {\r\nwidth: 35em;\r\nmargin: 0 auto;\r\nfont-family: Tahoma, sans-serif;\r\ntext-align: center;\r\n}\r\n</style>");
			this->_content.append("</head>\r\n");
			this->_content.append("<body>\r\n");
			this->_content.append("<h1>Welcome to " + this->_request->getServerName() + "</h1>\r\n<hr>\r\n");
			this->_content.append("<p>If you see this page, the web server is successfully installed and working. Further configuration is required.</p>");
			this->_content.append("<p>Thank you for using our webserv</p>");
			this->_content.append("<hr>\r\n");
			this->_content.append("<p>Plus d'informations sur notre serveur web <a href=\"https://github.com/lucocozz/webserv\">ici</a></p>\r\n");
			this->_content.append("</body>\r\n</html>\r\n");
		}

		void			_buildAutoIndexPage(std::string rootPath, std::string path, std::string oldPath){
			this->_contentType = "text/html";
			this->_content.append("<!DOCTYPE html><html>\r\n<head>\r\n<meta charset='utf-8'>\r\n");
			this->_content.append("<title>Index of " + buildPathTo(oldPath, "/", "") + "</title>\r\n</head>\r\n<body>\r\n");
			this->_content.append("<h1>Index of " + buildPathTo(oldPath, "/", "") + "</h1>\r\n<hr>\r\n");
			DIR *rep = NULL;
			struct dirent *fileRead = NULL;
			rep = opendir(buildPathTo(rootPath, path, "").c_str());
			if (rep == NULL){
				this->_buildErrorPage(INTERNAL_SERVER_ERROR, "");
				return ;
			}
			else{
				this->_content.append(buildListingBlock(fileRead, rep, rootPath, path, this->_request->findHeader("Host")));
				if (closedir(rep) == -1){
					this->_buildErrorPage(INTERNAL_SERVER_ERROR, "");
					return;
				}
			}
			this->_content.append("<hr>\r\n");
			this->_content.append("</body>\r\n</html>\r\n");
		}

		/*
			Upload a ressource :
		*/

		void	_uploadContent(const std::string &path, const Server &server, const std::pair<std::string, std::string> &clientInfo, const std::map<std::string, std::string> &headers, std::string &oldPath){
			if (this->_status / 100 != 2){
				this->_buildErrorPage(this->_status, "");
				return;
			}

			std::pair<bool,LocationContext> locationResult = 
				cgiChecker(this->_request->getPath(), server.context.locations);
			std::pair<bool, std::string> uploadLocation = retrieveLocationUpload(this->_request->getLocations(), oldPath);

			bool filenameBool = false;
			std::string contentDispo = this->_request->findHeader("Content-Disposition");
			if (contentDispo.empty() == false){
				if (contentDispo.find("filename=\"") != std::string::npos)
					filenameBool = true;
				else
					filenameBool = false;
			}

			if (locationResult.first == true){
				try{
					std::pair<std::string, int> cgiResponse;
					std::pair<ServerContext, LocationContext > serverLocation = 
						std::make_pair(server.context, locationResult.second);

					CGI cgi(path, headers, this->_request->getBody() , serverLocation, clientInfo, "POST");
					cgiResponse = cgi.cgiHandler();
					this->_content.append(cgiResponse.first);
					this->_contentType = "text/html";
					this->_status = cgiResponse.second;
				}
				catch(const std::exception &e){
					std::string exception(e.what());
					this->_status = INTERNAL_SERVER_ERROR;
					std::cerr << "Cgi failed: " << exception << std::endl;
				}
			}
			//MULTIPART
			else if (this->_request->getBoundarie().first == true){
				std::string locationName = retrieveUploadLocationName(this->_request->getLocations(), oldPath);
				if (uploadLocation.first == true){
					oldPath = buildPathTo(oldPath, uploadLocation.second, "");
					oldPath.replace(0, locationName.size(), this->_locationRootPath);
				}
				else if (locationName.empty() == false && locationName != this->_locationRootPath)
					oldPath.replace(0, locationName.size(), this->_locationRootPath);
				if (isPathValid(buildPathTo(this->_request->getRootPath(), oldPath, "")) == false){
					this->_buildErrorPage(NOT_FOUND, "");
					return;
				}

				for (std::map<std::map<std::string, std::string>, std::string>::const_iterator it = this->_request->getBodyMultipart().begin(); it != this->_request->getBodyMultipart().end(); it++)
					_uploadFileContent(oldPath, (*(*it).first.find("filename")).second, (*it).second);
			}
			//CONTENT DISPOSITION
			else if (filenameBool == true){
				std::string contentDisposition = (*this->_request->getHeaders().find("Content-Disposition")).second;
				std::string filename = contentDisposition.substr(contentDisposition.find("filename=\"") + 10);
				filename.erase(filename.end() - 1);

				std::string locationName = retrieveUploadLocationName(this->_request->getLocations(), oldPath);
				if (uploadLocation.first == true){
					oldPath.insert(oldPath.find(locationName) + locationName.size(), uploadLocation.second);
					oldPath.replace(0, locationName.size(), this->_locationRootPath);
				}
				else{
					oldPath.erase(0, locationName.size());
					oldPath = buildPathTo(this->_locationRootPath, oldPath, "");
				}
				if (isPathValid(buildPathTo(this->_request->getRootPath(), oldPath, "")) == false){
					this->_buildErrorPage(NOT_FOUND, "");
					return;
				}
				_uploadFileContent(oldPath, filename, this->_request->getBody());
			}
			else
				this->_status = UNPROCESSABLE_ENTITY;
			return;
		}

		void			_uploadFileContent(std::string const &oldPath, std::string const &filename, std::string const &body){
			std::string pathToFile;
			pathToFile = buildPathTo(this->_request->getRootPath(), oldPath, filename);
			if (isPathValid(pathToFile) == false)
				this->_status = CREATED;
			else
				this->_status = OK;
			if (this->getMimeTypes(pathToFile.c_str()).find("text/") != std::string::npos){
				std::ofstream outdata(pathToFile.c_str());
				outdata << body;
				outdata.close();
			}
			else{
				std::ofstream outdata(pathToFile.c_str(), std::ios::binary);
				outdata << body;
				outdata.close();
			}
		}

		/*

		*/

		bool						_isMethodAllowed(std::vector<LocationContext> locations, std::string path, std::string method, std::vector<std::string> allowedRoot){
			std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "allowed_method");
			if (isLocation.first == false && allowedRoot.empty() == false){
				for (size_t i = 0; i < allowedRoot.size(); i++){
					if (method == allowedRoot.at(i))
						return (true);
				}
				return (false);
			}
			else if (isLocation.first == false)
				return (true);

			std::vector<std::string> allowedMethods = retrieveDirectiveArgs(isLocation.second, "allowed_method");
			for (size_t i = 0; i < allowedMethods.size(); i++){
				if (method == allowedMethods.at(i))
					return (true);
			}
			this->_allowedLocation = allowedMethods;
			return (false);
		}

		/*
			Delete a ressource :
		*/

		void	_deleteContent(std::string oldPath){
			if (this->_status / 100 != 2){
				this->_buildErrorPage(this->_status, "");
				return;
			}

			if (this->_request->getPath() == "/")
				this->_buildErrorPage(FORBIDDEN, "");
			else if (this->_isMethodAllowed(this->_request->getLocations(), oldPath, this->_request->getMethod(), this->_request->getAllowedMethod()) == true){
				if (isPathValid(_rootToFile) && isPathDirectory(_rootToFile) == false)
					remove(_rootToFile.c_str());
				else if (isPathValid(_rootToFile) && isPathDirectory(_rootToFile) == true){
					if (removeDir(_rootToFile.c_str()) == -1)
						this->_buildErrorPage(FORBIDDEN, "");
				}
				else
					this->_buildErrorPage(NOT_FOUND, "");
			}
			else
				this->_buildErrorPage(METHOD_NOT_ALLOWED, "");
		}

		/*
			Error pages :
			- Build an error page (custom or default)
			- Build a default error page (generated)
		*/

		void					_buildErrorPage(int status, std::string customMessage){
			(void)customMessage;
			std::string ret;

			this->_content.clear();
			this->_status = status;
			if (this->_request->getErrorPage().second == true && customMessage.empty() == true){
				std::map<std::string, std::string> errorPages = this->_request->getErrorPage().first;
				std::map<std::string, std::string>::iterator it = errorPages.begin();
				for (; it != errorPages.end(); it++){
					if (match(itos(status).c_str(), (*it).first.c_str(), 'x') == 1)
						break;
				}
				if (it == errorPages.end()){
					this->_buildDefaultErrorPage(customMessage);
					return;
				}
				this->_retrieveFileContent(buildPathTo(this->_request->getRootPath(), (*it).second, ""));
				return;
			}
			this->_buildDefaultErrorPage(customMessage);
		}

		void					_buildDefaultErrorPage(std::string customMessage){
			std::string title;
			if (customMessage.empty() == true)
				title = itos(this->_status) + " " + this->getStatusMessage();
			else
				title = itos(this->_status) + " " + customMessage;
			this->_contentType = "text/html";
			this->_content.clear();
			this->_content.append("<!DOCTYPE html>\r\n<html>\n<head><meta charset='utf-8'>\r\n<title>" + title + "</title>");
			this->_content.append("<style>\r\nbody {\r\nwidth: 35em;\r\nmargin: 0 auto;\r\nfont-family: Tahoma, sans-serif;\r\ntext-align: center;\r\n}\r\n</style>");
			this->_content.append("</head>\n");
			this->_content.append("<body>\n<center><h1>" + title + "</h1></center>\n");
			this->_content.append("<hr><center>42webserv/0.0.1</center>\n");
			this->_content.append("</body>\n</html>");
		}

		/*
			Variables :
		*/

		httpRequest							*_request;

		int									_status;
		std::map<int, std::string>			_statusMessages;
		std::map<std::string, std::string>	_extensionTypes;
		std::string							_content;
		std::string							_contentType;
		std::string							_rootToFile;
		std::string							_locationRootPath;
		std::string							_redirectionHeader;
		std::vector<std::string>			_allowedLocation;
		std::string							_response;
};//end of class httpResponse

#endif
