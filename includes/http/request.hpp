/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:46 by user42            #+#    #+#             */
/*   Updated: 2022/04/11 19:03:01 by user42           ###   ########.fr       */
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

#include "../server/Server.hpp"
#include "../config/Config.hpp"
#include "statusCode.hpp"
#include "utils.hpp"

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

		std::string 								getMethod() const{return (this->_method);}

		std::string 								getPath() const{return (this->_path);}

		std::string 								getProtocol() const{return (this->_protocol);}

		std::map<std::string, std::string>			getHeaders() const{return (this->_headers);}

		std::string									getBody() const{return (this->_body);}

		int											getStatus() const{return (this->_status);}

		std::string									getServerName()	const{return (this->_serverName);}

		std::string									getRootPath() const{return (this->_rootPath);}

		std::string									getIndex() const{return (this->_index);}

		bool										getAutoindex() const{return (this->_autoindex);}

		size_t										getMaxBodySize() const{return (this->_maxBodySize);}

		std::pair<std::vector<std::string>, bool> 	getErrorPage() const{return (this->_errorPage);}

		size_t										getBodySize() const{return (this->_bodySize);}

		std::vector<LocationContext>				getLocations() const{return (this->_locations);}


	/*
		Underluying functions :
	*/

	private:

		/*
			Get config info :
			-	Retrieve the needed config info
		*/

		void	_retrieveConfigInfo(Server const &server){
			//server name
			try{
				std::vector<std::string> server_name = server.context.directives.at("server_name");
				this->_serverName = server_name[0];
			}
			catch (std::exception const &e){this->_serverName = "server";}
			//root
			try{
				std::vector<std::string> root_path = server.context.directives.at("root");
				this->_rootPath = root_path[0];
			}
			catch (std::exception const &e){this->_rootPath = "/";}
			//server index
			try{
				std::vector<std::string> index = server.context.directives.at("index");
				this->_index = checkIndex(_rootPath, index);
			}
			catch (std::exception const &e){this->_index = "default_index.html";}
			//auto index
			try{
				std::string autoindex = server.context.directives.at("autoindex")[0];
				if (autoindex == "on")
					this->_autoindex = true;
				else
					this->_autoindex = false;
			}
			catch (std::exception const &e){this->_autoindex = false;}
			//client max body size
			try{
				if (atoi((server.context.directives.at("client_max_body_size")[0]).c_str()) < 100)
					this->_maxBodySize = atoi((server.context.directives.at("client_max_body_size")[0]).c_str()) * 1000;
				else
					this->_maxBodySize = 1 * 1000;
			}
			catch (std::exception const &e){this->_maxBodySize = 1 * 1000;}
			//error pages
			try{
				this->_errorPage.first = server.context.directives.at("error_page");
				this->_errorPage.second = true;
			}
			catch (std::exception const &e){this->_errorPage.second = false;}

			//location index & limit_except
			{
				std::vector<LocationContext> locations = server.context.locations;
				for (std::vector<LocationContext>::iterator it = locations.begin(); it != locations.end(); it++){
					int needDel = 0;
					//Index
					try {std::vector<std::string> vec = (*it).directives.at("index");}
					catch (std::exception const &e){needDel++;}
					//Limit Except
					try {std::vector<std::string> vec = (*it).directives.at("limit_except");}
					catch (std::exception const &e){needDel++;}
					//Delete useless locations
					if (needDel == 2){
						locations.erase(it);
						if (locations.empty() == true)
							break;
						it = locations.begin();
					}
				}
				this->_locations = locations;
			}
		}

		/*
			Parsing :
			-	Retrieve the body first
			-	Retrieve the request line (Method + Path + Protocol)
			-	Retrieve the headers in a map
		*/

		void	_parse(std::string const &rawRequest){
			std::string rawHeaders;
			rawHeaders.append(rawRequest.substr(0, rawRequest.find("\r\n\r\n")));
			std::vector<std::string> vecHeaders = split(rawHeaders, "\r\n");

			if (vecHeaders.size() > 0){
				//RequestLine
				std::vector<std::string> requestLine = split(vecHeaders.at(0), " ");
				if (requestLine.size() == 3){
					this->_method = requestLine.at(0);
					this->_path = requestLine.at(1);
					this->_protocol = requestLine.at(2);
					vecHeaders.erase(vecHeaders.begin());
				}
				else{
					this->_status = BAD_REQUEST;
					return;
				}
				//Headers map
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
				//Body
				std::string rawBody;
				rawBody.append(rawRequest.substr(rawRequest.find("\r\n\r\n") + 4));
				this->_body = rawBody;
				this->_bodySize = this->_body.size();
			}
			else{
				this->_status = BAD_REQUEST;
				return;
			}
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
		std::vector<std::string> 						_request;

		//Config
		std::string										_serverName;
		std::string										_rootPath;
		std::string										_index;
		bool											_autoindex;
		std::pair<std::vector<std::string>, bool>		_errorPage;
		size_t											_maxBodySize;
		std::vector<LocationContext>					_locations;

		//Request
		size_t											_bodySize;
		std::string 									_method;
		std::string 									_path;
		std::string 									_protocol;
		std::map<std::string, std::string>				_headers;
		std::string										_body;
		int												_status;
};//end of class httpRequest

#endif
