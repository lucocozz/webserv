#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include <sys/stat.h>

#include "httpStatus.hpp"
#include "httpUtils.hpp"

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
			this->_method = rhs.getMethod();
			this->_path = rhs.getPath();
			this->_protocol = rhs.getProtocol();
			this->_headers = rhs.getHeaders();
			this->_body = rhs.getBody();
			this->_status = rhs.getStatus();
			return (*this);
		}

		void	treatRequest(std::string const &rawRequest){
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

		int									getStatus() const{
			return (this->_status);
		}


	private:

		//Parsing
		void	_parse(std::string const &rawRequest){
			_parseBody(rawRequest);
			if (_request.size() != 0){
				_parseRequestLine();
				_parseHeaders();
			}
			else
				this->_status = BAD_REQUEST;
		}

		void	_parseBody(std::string const &rawRequest){
			std::vector<std::string> sep = split(rawRequest, "\r\n\r\n");
			if (sep.size() == 2)
				this->_body = sep.at(1);
			this->_request = split(sep.at(0), "\r\n");
		}

		void	_parseRequestLine(){
			std::vector<std::string> requestLine = split(this->_request.at(0), " ");
			if (requestLine.size() == 3){
				this->_method = requestLine.at(0);
				this->_path = requestLine.at(1);
				this->_protocol = requestLine.at (2);
				this->_request.erase(_request.begin());
			}
			else
				_status = BAD_REQUEST;
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
			if (this->_status != OK)
				return;
			this->_checkRequestLine();
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
			if (isPathExist(_path) == false){
				this->_status = NOT_FOUND;
				return;
			}
			if (this->_protocol != "HTTP/1.1"){
				this->_status = HTTP_VERSION_NOT_SUPPORTED;
				return;
			}
		}

		//Members
		std::vector<std::string> 			_request;

		//Request
		std::string 						_method;
		std::string 						_path;
		std::string 						_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;
		int									_status;
};//end of class httpRequest

#endif