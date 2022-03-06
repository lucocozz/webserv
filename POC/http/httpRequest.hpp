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
			_status = OK;
			return;
		}

		~httpRequest(){
			return;
		}

		void	treatData(std::string const &rawRequest){
			_parse(rawRequest);
			//debug
			std::cout << std::endl;
			std::cout << "_method = " << _method << std::endl;
			std::cout << "_path = " << _path << std::endl;
			std::cout << "_protocol = " << _protocol << std::endl;
			for (std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); it++)
				std::cout << "_headers[] = {'" << (*it).first << "' , '" << (*it).second << "'}" << std::endl;
			std::cout << _body << std::endl;
			_check();
			_answer();
			//debug
			std::cout << "RESPONSE :" << std::endl;
			std::cout << getResponse() << std::endl;
		}

		std::string		findHeader(std::string key){
			std::map<std::string, std::string>::iterator it = _headers.find(key);
			if (it == _headers.end())
				return (std::string());
			return ((*it).second);
		}

		std::string		getResponse() const{
			return (_response);
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
				_status = BAD_REQUEST;
		}

		void	_parseBody(std::string const &rawRequest){
			std::vector<std::string> sep = split(rawRequest, "\r\n\r\n");
			if (sep.size() == 2)
				_body = sep.at(1);
			_request = split(sep.at(0), "\r\n");
		}

		void	_parseRequestLine(){
			std::vector<std::string> requestLine = split(_request.at(0), " ");
			if (requestLine.size() == 3){
				_method = requestLine.at(0);
				_path = requestLine.at(1);
				_protocol = requestLine.at (2);
				_request.erase(_request.begin());
			}
			else
				_status = BAD_REQUEST;
		}

		void	_parseHeaders(){
			if (_status != OK)
				return;
			for (size_t i = 0; i < _request.size(); i++){
				if (_request.at(i).find(": ") == std::string::npos){
					_status = BAD_REQUEST;
					return;
				}
				std::vector<std::string> res = split(_request.at(i), ": ");
				if (res.size() < 3){
					std::pair<std::string, std::string> pair = std::make_pair(res.at(0), res.at(1));
					_headers.insert(pair);
				}
				else{
					_status = BAD_REQUEST;
					return;
				}
				res.clear();
			}
		}

		//Checking
		void	_check(){
			if (_status != OK)
				return;
			_checkRequestLine();
		}

		void	_checkRequestLine(){
			if (_method != "GET" && _method != "POST" && _method != "DELETE"){
				_status = NOT_IMPLEMENTED;
				return;
			}
			else if (_method == "POST" && _body.empty() == true){
				std::map<std::string, std::string>::iterator it(_headers.find("Content-Length"));
				if (it == _headers.end()){
				_status = LENGTH_REQUIRED;
				return;
				}
			}
			if (isPathExist(_path) == false){
				_status = NOT_FOUND;
				return;
			}
			if (_protocol != "HTTP/1.1"){
				_status = HTTP_VERSION_NOT_SUPPORTED;
				return;
			}
		}

		//Answering
		void	_answer(){
			_buildStatusLine();
		}

		void _buildStatusLine(){
			_response.append("HTTP/1.1 ");
			_response.append(itos(_status) + " ");
			_response.append(getStatusMessage(_status) + "\r\n");
		}

		//Members
		std::vector<std::string> 			_request;

		//Request
		std::string 						_method;
		std::string 						_path;
		std::string 						_protocol;
		std::map<std::string, std::string>	_headers;
		std::string							_body;

		//Response
		int									_status;
		std::string							_response;
};//end of class httpRequest

#endif