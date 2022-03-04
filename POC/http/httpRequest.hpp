#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include <sys/stat.h>

//OK
#define OK 200
//Error client
#define BAD_REQUEST 400
#define NOT_FOUND 404
#define LENGTH_REQUIRED 411
//Error server
#define INTERNAL_SERVER_ERROR 500
#define NOT_IMPLEMENTED 501
#define SERVICE_UNAVAILABLE 503
#define HTTP_VERSION_NOT_SUPPORTED 505


std::vector<std::string>	split(std::string str, std::string delimiter){
	size_t posStart = 0;
	size_t posEnd = 0;
	size_t delimLen = delimiter.length();
	std::string token;
	std::vector<std::string> ret;

	while ((posEnd = str.find(delimiter, posStart)) != std::string::npos){
		token = str.substr(posStart, posEnd - posStart);
		posStart = posEnd + delimLen;
		ret.push_back(token);
	}
	ret.push_back(str.substr(posStart));
	return (ret);
}

bool						isPathExist(std::string const &path){
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

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
			//debug
			std::cout << "findHeader(Host) = '" << findHeader("Host") << "'" << std::endl;
			std::cout << "findHeader(Content-Length) = '" << findHeader("Content-Length") << "'" << std::endl;
			std::cout << "STATUS CODE : " << _status << std::endl << std::endl;
		}

		std::string		findHeader(std::string key){
			std::map<std::string, std::string>::iterator it = _headers.find(key);
			if (it == _headers.end())
				return (std::string());
			return ((*it).second);
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

		std::vector<std::string> 			_request;

		std::string 						_method;
		std::string 						_path;
		std::string 						_protocol;

		std::map<std::string, std::string>	_headers;

		std::string							_body;
		int									_status;
};//end of class httpRequest

#endif