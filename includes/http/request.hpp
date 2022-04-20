/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:46 by user42            #+#    #+#             */
/*   Updated: 2022/04/20 01:59:09 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP


/*
	Includes :
*/

#include "configData.hpp"
#include "fileRelated.hpp"
#include "pathRelated.hpp"
#include "statusCode.hpp"
#include "stringRelated.hpp"
#include "URLRelated.hpp"
#include "locationRelated.hpp"

/*
	httpRequest class :
*/

class httpRequest{
	public:

		/*
			Coplien form :
			-	Default constructor
			-	Copy constructor
			-	Destructor
			-	Assignement operator
		*/

		httpRequest(){
			this->_status = OK;
			return;
		}

		httpRequest(httpRequest const &src){
			*this = src;
			return;
		}

		~httpRequest(){
			return;
		}

		httpRequest &operator=(httpRequest const &rhs){
			this->_serverName = rhs.getServerName();
			this->_rootPath = rhs.getRootPath();
			this->_index = rhs.getIndex();
			this->_autoindex = rhs.getAutoindex();
			this->_errorPage = rhs.getErrorPage();
			this->_maxBodySize = rhs.getMaxBodySize();
			this->_bodyMultipart = rhs.getBodyMultipart();
			this->_boundarie = rhs.getBoundarie();
			this->_bodySize = rhs.getBodySize();
			this->_locations = rhs.getLocations();

			this->_method = rhs.getMethod();
			this->_path = rhs.getPath();
			this->_protocol = rhs.getProtocol();
			this->_headers = rhs.getHeaders();
			this->_body = rhs.getBody();
			this->_status = rhs.getStatus();

			this->_chunked = rhs.getChunked();
			this->_concatenedRequest = rhs.getConcatenedRequest();
			this->_contentLength = rhs.getContentLenght();
			return (*this);
		}

		/*
			Public functions :
			- Parse and check the request
			- Return the specified header if found
		*/

		static size_t	getHeaderContentLenght(std::string &request){
			std::cout << "getheadercontentlength\n";
			std::string::iterator 	itb = request.begin() + request.find("Content-Length:") + 16;
			std::string::iterator 	ite = itb;
			std::string				contentLengthStr("");

			while (isdigit(*ite))
				ite++;
			contentLengthStr.append(itb, ite);
			return (atoi(contentLengthStr.c_str()));
		}

		bool								treatRequest(std::string &rawRequest, Server const &server){

			//check if the request is POST, if it is I set contentLength
			if (rawRequest.find("POST") != std::string::npos){
				this->_concatenedRequest.clear();
				if (rawRequest.find("Transfer-Encoding: chunked") != std::string::npos)
					this->_chunked = true;
				else
					this->_contentLength = getHeaderContentLenght(rawRequest);
			}
			else if (rawRequest.find("GET") != std::string::npos){
				this->_concatenedRequest.clear();
				this->_contentLength = rawRequest.size();
			}
			//While the concatened request is smaller than the header content-length I keep concatenating || if the chunked bool is still true
			if (this->_concatenedRequest.size() < this->_contentLength || this->_chunked == true)
				this->_concatenedRequest.append(rawRequest);
			//If there is 0\r\n at the end of a chunked request chunked bool is set to false which means that the request has been fully received
			if (this->_concatenedRequest.find("0\r\n", this->_concatenedRequest.size() - 5) != std::string::npos && this->_chunked == true)
				this->_chunked = false;
			//If the request has been fully received we treat it and build the response then set all the statics to default
			if (this->_concatenedRequest.size() >= this->_contentLength && this->_chunked == false){
				this->_retrieveConfigInfo(server);
				this->_parse(this->_concatenedRequest);
				this->_check();
				return (true);
			}
			return (false);
		}

		std::string							findHeader(std::string key){
			std::map<std::string, std::string>::iterator it = this->_headers.find(key);
			if (it == this->_headers.end())
				return (std::string());
			return ((*it).second);
		}

		void					clear(){
			this->_request.clear();
			this->_serverName.clear();
			this->_rootPath.clear();
			this->_index.clear();
			this->_autoindex = false;
			this->_errorPage.second = false;
			this->_maxBodySize = 0;
			this->_locations.clear();
			this->_boundarie.first = false;
			this->_bodyMultipart.clear();
			this->_body.clear();
			this->_bodySize = 0;
			this->_filename.clear();
			this->_method.clear();
			this->_path.clear();
			this->_protocol.clear();
			this->_headers.clear();
			this->_status = 200;
			this->_chunked = false;
			this->_contentLength = 0;
			this->_concatenedRequest.clear();
		}

		/*
			Getters :
		*/

		const std::string 											&getMethod() const{return (this->_method);}

		const std::string 											&getPath() const{return (this->_path);}

		void														setPath(std::string path){
			this->_path = path;
		}

		const std::string 											&getProtocol() const{return (this->_protocol);}

		const std::map<std::string, std::string>					&getHeaders() const{return (this->_headers);}

		const std::string											&getBody() const{return (this->_body);}

		const int													&getStatus() const{return (this->_status);}

		const std::string											&getServerName()	const{return (this->_serverName);}

		const std::string											&getRootPath() const{return (this->_rootPath);}

		//void														setRootPath(std::string rootPath){
		//	this->_rootPath = rootPath;
		//}

		const std::string											&getIndex() const{return (this->_index);}

		const bool													&getAutoindex() const{return (this->_autoindex);}

		const size_t												&getMaxBodySize() const{return (this->_maxBodySize);}

		const std::pair<std::map<std::string, std::string>, bool> 	&getErrorPage() const{return (this->_errorPage);}

		const std::pair<bool, std::string>							&getBoundarie() const{return (this->_boundarie);}

		const std::map<std::map<std::string, std::string>, std::string> 		&getBodyMultipart() const{return (this->_bodyMultipart);}

		const size_t												&getBodySize() const{return (this->_bodySize);}

		const std::vector<LocationContext>							&getLocations() const{return (this->_locations);}

		const std::string											&getConcatenedRequest() const {return (this->_concatenedRequest);}

		const size_t												&getContentLenght() const {return (this->_contentLength);}
 
		const bool													&getChunked() const {return (this->_chunked);}

	/*
		Underluying functions :
	*/

	private:

		/*
			Get config info :
			-	Retrieve the needed config info
		*/

		void	_retrieveConfigInfo(Server const &server){
			this->_serverName = getConfigServerName(server);
			this->_rootPath = getConfigRootPath(server);
			this->_index = getConfigIndex(server, _rootPath);
			this->_autoindex = getConfigAutoIndex(server);
			this->_maxBodySize = getConfigMaxBodySize(server);
			this->_errorPage = getConfigErrorPage(server);
			this->_locations = getConfigLocations(server);
		}

		/*
			Parsing :
			-	Parse the request
			-	Parse the request line part
			-	Parse the headers
		*/

		void	_parse(std::string const &rawRequest){
			std::string rawHeaders;
			rawHeaders.append(rawRequest.substr(0, rawRequest.find("\r\n\r\n")));
			std::vector<std::string> vecHeaders = split(rawHeaders, "\r\n");

			if (vecHeaders.size() > 0){
				this->_parseRequestLine(vecHeaders);
				this->_parseHeaders(vecHeaders);
				this->_parseBody(rawRequest);
			}
			else{
				this->_status = BAD_REQUEST;
				return;
			}
		}

		void	_parseRequestLine(std::vector<std::string> &vecHeaders){
			std::vector<std::string> requestLine = split(vecHeaders.at(0), " ");
			if (requestLine.size() == 3){
				this->_method = requestLine.at(0);
				this->_path = requestLine.at(1);
				for (std::string::iterator it = this->_path.end() - 1; it != this->_path.begin() && *it == '/'; it--){
					this->_path.erase(it);
					it = this->_path.end() - 1;
				}
				this->_protocol = requestLine.at(2);
				vecHeaders.erase(vecHeaders.begin());
			}
			else{
				this->_status = BAD_REQUEST;
				return;
			}
		}

		void	_parseHeaders(std::vector<std::string> &vecHeaders){
			std::map<std::string, std::string>				mapHeaders;
			for (size_t i = 0; i < vecHeaders.size(); i++){
				std::vector<std::string> res = split(vecHeaders.at(i), ": ");
				if (res.size() == 2){
					if (res.at(1).find("; boundary=") != std::string::npos){
						this->_boundarie.first = true;
						this->_boundarie.second = "--" + res.at(1).substr(res.at(1).find("; boundary=") + 11, res.at(1).size() - 2);
					}
					std::pair<std::string, std::string> pair = std::make_pair(res.at(0), res.at(1));
					mapHeaders.insert(pair);
				}
				else{
					this->_status = BAD_REQUEST;
					return;
				}
				res.clear();
			}
			this->_headers = mapHeaders; 
		}

		void		_parseBody(std::string rawRequest){
			//If multipart
			if (this->_boundarie.first == true){
				std::string rawBody = rawRequest.substr(rawRequest.find("\r\n\r\n") + 4);
				std::vector<std::string> separedMultipart = split(rawBody, this->_boundarie.second + "\r\n");

				for (std::vector<std::string>::iterator it = separedMultipart.begin(); it != separedMultipart.end(); it++){
					if ((*it).empty() == true){
						separedMultipart.erase(it);
						it = separedMultipart.begin();
					}
				}

				for (size_t i = 0; i < separedMultipart.size(); i++){
					std::pair<std::map<std::string,std::string>, std::string>	pair;
					std::vector<std::string> tmpVec = split(separedMultipart.at(i).substr(0, separedMultipart.at(i).find("\r\n\r\n")), "\r\n");
					for (std::vector<std::string>::iterator itt = tmpVec.begin(); itt != tmpVec.end(); itt++){
						if ((*itt).find("; ") != std::string::npos){
							std::vector<std::string> splitedHeader = split(*itt, "; ");
							for (size_t x = 0; x < splitedHeader.size(); x++){
								if (splitedHeader.at(x).find(": ") != std::string::npos){
									std::vector<std::string> keyValue = split(splitedHeader.at(x), ": ");
									pair.first.insert(std::make_pair(keyValue.at(0), keyValue.at(1)));
								}
								else if (splitedHeader.at(x).find("=\"") != std::string::npos){
									std::vector<std::string> keyValue = split(splitedHeader.at(x), "=\"");
									pair.first.insert(std::make_pair(keyValue.at(0), keyValue.at(1).substr(0, keyValue.at(1).find("\""))));
								}
							}
						}
						else{
							if ((*itt).find(": ") != std::string::npos){
								std::vector<std::string> keyValue = split((*itt), ": ");
								pair.first.insert(std::make_pair(keyValue.at(0), keyValue.at(1)));
							}
							else if ((*itt).find("=\"") != std::string::npos){
								std::vector<std::string> keyValue = split((*itt), "=\"");
								pair.first.insert(std::make_pair(keyValue.at(0), keyValue.at(1).substr(0, keyValue.at(1).find("\""))));
							}
						}
					}
					//End of multipart
					if (separedMultipart.at(i).find(_boundarie.second + "--") != std::string::npos){
						pair.second = separedMultipart.at(i).substr(separedMultipart.at(i).find("\r\n\r\n") + 4);
						pair.second = pair.second.substr(0, pair.second.find(_boundarie.second + "--"));
					}
					//Not ended
					else
						pair.second = separedMultipart.at(i).substr(separedMultipart.at(i).find("\r\n\r\n") + 4);
					this->_bodyMultipart.insert(pair);
				}
				this->_body.append(rawBody);
				this->_bodySize = this->_body.size();
			}
			//If content is identity
			else{
				this->_body.append(rawRequest.substr(rawRequest.find("\r\n\r\n") + 4));
				this->_bodySize = this->_body.size();
			}
		}

		/*
			Checking :
		*/

		void	_check(){
			if (this->_status / 100 == 4 || this->_status / 100 == 5)
				return;
			this->_checkRequestLine();

			//Check if there is a multipart without "filename";
			if (this->_boundarie.first == true){
				for (std::map<std::map<std::string, std::string>, std::string>::const_iterator it = this->_bodyMultipart.begin(); it != this->_bodyMultipart.end(); it++){
					if ((*it).first.find("filename") == (*it).first.end()){
						this->_status = UNPROCESSABLE_ENTITY;
						return;
					}
				}
			}
			//Check if the body size exceed the max body size
			// if (this->_maxBodySize < this->_bodySize){
			// 	this->_status = REQUEST_ENTITY_TOO_LARGE;
			// 	return;
			// }
			//Check if the host is specified
			if (this->findHeader("Host").empty() == true){
				this->_status = BAD_REQUEST;
				return;
			}
		}

		void	_checkRequestLine(){
			if (this->_method != "GET" && this->_method != "POST" && this->_method != "DELETE"){
				this->_status = NOT_IMPLEMENTED;
				return;
			}
			else if (this->_method == "POST" && this->_body.empty() == true){
				std::map<std::string, std::string>::iterator it(this->_headers.find("Content-Length"));
				if (it == _headers.end()){
					this->_status = LENGTH_REQUIRED;
					return;
				}
			}
			if (this->_protocol != "HTTP/1.1"){
				this->_status = HTTP_VERSION_NOT_SUPPORTED;
				return;
			}
		}

		/*
			Variables :
		*/

		std::string											_concatenedRequest;
		size_t												_contentLength;
		bool												_chunked;

		//rawRequest
		std::vector<std::string> 							_request;

		//Config
		std::string											_serverName;
		std::string											_rootPath;
		std::string											_index;
		bool												_autoindex;
		std::pair<std::map<std::string, std::string>, bool> _errorPage;
		size_t												_maxBodySize;
		std::vector<LocationContext>						_locations;

		//Request
		std::pair<bool, std::string>						_boundarie;
		std::map<std::map<std::string, std::string>, std::string> 	_bodyMultipart;
		std::string											_body;
		size_t												_bodySize;
		std::string											_filename;

		std::string 										_method;
		std::string 										_path;
		std::string 										_protocol;
		std::map<std::string, std::string>					_headers;

		int													_status;
};//end of class httpRequest

#endif
