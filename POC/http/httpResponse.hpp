#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "httpRequest.hpp"

class httpResponse{
	public:
		httpResponse(){
			this->_setStatusMessage();
			return;
		}

		~httpResponse(){
			return;
		}

		void	buildResponse(httpRequest const &request){
			this->_request = request;
			_answer();
			std::cout << "RESPONSE :" << std::endl;
			std::cout << _response << std::endl;
		}

	private:
		//init
		void	_setStatusMessage(){
			this->_statusMessage.insert(std::make_pair(OK, "OK"));
			this->_statusMessage.insert(std::make_pair(CREATED, "Created"));
			this->_statusMessage.insert(std::make_pair(BAD_REQUEST, "Bad Request"));
			this->_statusMessage.insert(std::make_pair(NOT_FOUND, "Not Found"));
			this->_statusMessage.insert(std::make_pair(LENGTH_REQUIRED, "Length Required"));
			this->_statusMessage.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
			this->_statusMessage.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
			this->_statusMessage.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
			this->_statusMessage.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "http Version Not Supported"));
		}

		std::string	_getStatusMessage(){
			return ((*_statusMessage.find(this->_request.getStatus())).second);
		}

		//Answering
		void	_answer(){
			this->_buildStatusLine();
		}

		void _buildStatusLine(){
			this->_response.append("HTTP/1.1 ");
			this->_response.append(itos(this->_request.getStatus()) + " ");
			//this->_response.append(_getStatusMessage() + "\r\n");
			this->_response.append(getStatusMessage(this->_request.getStatus()) + "\r\n");
		}

		httpRequest						_request;
		std::map<int, std::string> 		_statusMessage;
		std::string						_response;
};//end of class httpResponse

#endif