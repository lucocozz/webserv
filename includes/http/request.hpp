/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:46 by user42            #+#    #+#             */
/*   Updated: 2022/04/14 23:07:18 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

/*
	Includes :
*/

#include <algorithm>
#include <vector>
#include <map>
#include <utility>
#include <sys/stat.h>

#include "Server.hpp"
#include "Config.hpp"
#include "configData.hpp"
#include "statusCode.hpp"
#include "utils/fileRelated.hpp"

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
			this->_bodySize = rhs.getBodySize();
			this->_locations = rhs.getLocations();

			this->_method = rhs.getMethod();
			this->_path = rhs.getPath();
			this->_protocol = rhs.getProtocol();
			this->_headers = rhs.getHeaders();
			this->_body = rhs.getBody();
			this->_status = rhs.getStatus();
			return (*this);
		}

		/*
			Public functions :
			- Parse and check the request
			- Return the specified header if found
		*/

		void								treatRequest(std::string const &rawRequest, Server const &server){
			this->_retrieveConfigInfo(server);
			this->_parse(rawRequest);
			this->_check();
		}

		std::string							findHeader(std::string key){
			std::map<std::string, std::string>::iterator it = this->_headers.find(key);
			if (it == this->_headers.end())
				return (std::string());
			return ((*it).second);
		}

		std::pair<bool, std::string>		findBoundarie(){
			std::map<std::string, std::string>::iterator it = this->_headers.find("Content-Type");
			std::string boundarie;
			bool ret = false;
			if (it == this->_headers.end())
				return (std::make_pair(ret, boundarie));
			else{
				if ((*it).second.find("multipart/form-data", 0, 18) != std::string::npos){
					std::vector<std::string> vec = split((*it).second, "; boundary=");
					if (vec.size() == 2){
						boundarie = vec.at(1);
						ret = true;
					}
				}
			}
			return (std::make_pair(ret, boundarie));
		}

		/*
			Getters :
		*/

		const std::string 											&getMethod() const{return (this->_method);}

		const std::string 											&getPath() const{return (this->_path);}

		const std::string 											&getProtocol() const{return (this->_protocol);}

		const std::map<std::string, std::string>					&getHeaders() const{return (this->_headers);}

		const std::string											&getBody() const{return (this->_body);}

		const int													&getStatus() const{return (this->_status);}

		const std::string											&getServerName()	const{return (this->_serverName);}

		const std::string											&getRootPath() const{return (this->_rootPath);}

		const std::string											&getIndex() const{return (this->_index);}

		const bool													&getAutoindex() const{return (this->_autoindex);}

		const size_t												&getMaxBodySize() const{return (this->_maxBodySize);}

		const std::pair<std::map<std::string, std::string>, bool> 	&getErrorPage() const{return (this->_errorPage);}

		const size_t												&getBodySize() const{return (this->_bodySize);}

		const std::vector<LocationContext>							&getLocations() const{return (this->_locations);}


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
				this->_body.append(rawRequest.substr(rawRequest.find("\r\n\r\n") + 4));
				this->_bodySize = this->_body.size();
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
				if (vecHeaders.at(i).find(": ") == std::string::npos){
					this->_status = BAD_REQUEST;
					return;
				}
				std::vector<std::string> res = split(vecHeaders.at(i), ": ");
				if (res.size() == 2){
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

		/*
			Checking :
		*/

		void	_check(){
			if (this->_status / 100 == 4 || this->_status / 100 == 5)
				return;
			this->_checkRequestLine();

			//Check if the body size exceed the max body size
			if (this->_maxBodySize < this->_bodySize){
				this->_status = REQUEST_ENTITY_TOO_LARGE;
				return;
			}
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
		size_t												_bodySize;
		std::string 										_method;
		std::string 										_path;
		std::string 										_protocol;
		std::map<std::string, std::string>					_headers;
		std::string											_body;
		int													_status;
};//end of class httpRequest

#endif
