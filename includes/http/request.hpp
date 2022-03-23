/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:46 by user42            #+#    #+#             */
/*   Updated: 2022/03/23 17:56:54 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include <sys/stat.h>

#include "../config/Config.hpp"

#include "statusCode.hpp"
#include "utils.hpp"

class httpRequest{
	public:
		httpRequest(){
			this->_status = OK;
			return;
		}

		~httpRequest(){
			return;
		}

		httpRequest &operator=(httpRequest const &rhs){
			//Config
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

		void	treatRequest(std::string const &rawRequest, Config const &config){
			this->_retrieveConfigInfo(config);
			this->_parse(rawRequest);

			this->_check();
		}

		std::string		findHeader(std::string key){
			std::map<std::string, std::string>::iterator it = this->_headers.find(key);
			if (it == this->_headers.end())
				return (std::string());
			return ((*it).second);
		}

		//Getters
		std::string 						getMethod() const{
			return (this->_method);
		}

		std::string 						getPath() const{
			return (this->_path);
		}

		std::string 						getProtocol() const{
			return (this->_protocol);
		}

		std::map<std::string, std::string>	getHeaders() const{
			return (this->_headers);
		}

		std::string							getBody() const{
			return (this->_body);
		}

		int											getStatus() const{
			return (this->_status);
		}

		std::string									getServerName()	const{
			return (this->_serverName);
		}

		std::string									getRootPath() const{
			return (this->_rootPath);
		}

		std::vector<std::string>					getIndex() const{
			return (this->_index);
		}

		bool										getAutoindex() const{
			return (this->_autoindex);
		}

		std::pair<size_t, bool>						getMaxBodySize() const{
			return (this->_maxBodySize);
		}

		std::pair<std::vector<std::string>, bool> getErrorPage() const{
			return (this->_errorPage);
		}

		size_t										getBodySize() const{
			return (this->_bodySize);
		}

	private:

		//Retrieve Config Info
		void	_retrieveConfigInfo(Config const &config){
			//Retrieve server name
			try{
				std::vector<std::string> server_name = config.servers[0].directives.at("server_name");
				this->_serverName = server_name[0];
			}
			catch (std::exception const &e){
				//need to check if this serverName already exist ?
				this->_serverName = "server";
			}

			//Retrieve server root
			try{
				std::vector<std::string> root_path = config.servers[0].directives.at("root");
				this->_rootPath = root_path[0];
			}
			catch (std::exception const &e){
				this->_rootPath = "/";
			}

			//Retrieve server index
			try{
				std::vector<std::string> index = config.servers[0].directives.at("index");
				this->_index = index;
			}
			catch (std::exception const &e){
				this->_index.push_back("default_index.html");
			}

			//Retrieve server autoindex
			try{
				std::string autoindex = config.servers[0].directives.at("autoindex")[0];
				if (autoindex == "on")
					this->_autoindex = true;
				else
					this->_autoindex = false;
			}
			catch (std::exception const &e){
				this->_autoindex = false;
			}

			//Retrieve the body max size
			try{
				this->_maxBodySize.first = atoi((config.servers[0].directives.at("client_max_body_size")[0]).c_str());
				this->_maxBodySize.second = true;
			}
			catch (std::exception const &e){
				this->_maxBodySize.second = false;
			}

			//Retrieve if config file include custom error pages
			try{
				this->_errorPage.first = config.servers[0].directives.at("error_page");
				this->_errorPage.second = true;
			}
			catch (std::exception const &e){
				this->_errorPage.second = false;
			}
		}

		//Parsing
		void	_parse(std::string const &rawRequest){
			this->_parseBody(rawRequest);
			if (this->_request.size() != 0){
				this->_parseRequestLine();
				this->_parseHeaders();
			}
			else
				this->_status = BAD_REQUEST;
		}

		void	_parseBody(std::string const &rawRequest){
			std::vector<std::string> sep = split(rawRequest, "\r\n\r\n");
			if (sep.size() == 2){
				this->_body = sep.at(1);
				this->_bodySize = _body.size();
			}
			this->_request = split(sep.at(0), "\r\n");
		}

		void	_parseRequestLine(){
			std::vector<std::string> requestLine = split(this->_request.at(0), " ");
			if (requestLine.size() == 3){
				this->_method = requestLine.at(0);
				this->_path = requestLine.at(1);
				this->_protocol = requestLine.at(2);
				this->_request.erase(_request.begin());
			}
			else
				this->_status = BAD_REQUEST;
		}

		void	_parseHeaders(){
			if (this->_status != OK)
				return;
			for (size_t i = 0; i < _request.size(); i++){
				if (this->_request.at(i).find(": ") == std::string::npos){
					this->_status = BAD_REQUEST;
					return;
				}
				std::vector<std::string> res = split(_request.at(i), ": ");
				if (res.size() == 2){
					std::pair<std::string, std::string> pair = std::make_pair(res.at(0), res.at(1));
					this->_headers.insert(pair);
				}
				else{
					this->_status = BAD_REQUEST;
					return;
				}
				res.clear();
			}
		}

		//Checking
		void	_check(){
			if (this->_status / 100 == 4 || this->_status / 100 == 5)
				return;
			this->_checkRequestLine();

			//this->_checkBody();
			//Check if the body size exceed the max body size
			if (this->_maxBodySize.second == true && this->_maxBodySize.first < this->_bodySize){
				this->_status = REQUEST_ENTITY_TOO_LARGE;
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
			//if (doesFileExist(this->_path) == false){
			//	this->_status = NOT_FOUND;
			//	return;
			//}
			if (this->_protocol != "HTTP/1.1"){
				this->_status = HTTP_VERSION_NOT_SUPPORTED;
				return;
			}
		}

		//rawRequest
		std::vector<std::string> 			_request;

		//Config
		std::string										_serverName;
		std::string										_rootPath;
		std::vector<std::string>						_index;
		bool											_autoindex;
		std::pair<std::vector<std::string>, bool>		_errorPage;
		std::pair<size_t, bool>							_maxBodySize;							

		//Request
		size_t								_bodySize;
		std::string 						_method;
		std::string 						_path;
		std::string 						_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_status;
};//end of class httpRequest

#endif