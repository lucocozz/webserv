#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "httpRequest.hpp"
#include "../server/Epoll.hpp"

class httpResponse{
	public:
		httpResponse(){
			this->_setStatusMessage();
			return;
		}

		~httpResponse(){
			return;
		}

		void	sendResponse(EpollSocket &socketEvent){
			socketEvent.sendData(_response);
		}

		void	buildResponse(httpRequest const &request){
			this->_request = request;
			//Get the content from the method;
			_content = "<!DOCTYPE html><html>Response sent by 42 webserv.</html>";
			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();

			std::cout << "RESPONSE :" << std::endl;
			std::cout << _response << std::endl;
		}

	private:

		//Build the response
		void _buildStatusLine(){
			this->_response.append("HTTP/1.1 ");
			this->_response.append(itos(this->_request.getStatus()) + " ");
			this->_response.append(_getStatusMessage() + "\r\n");
		}

		void _buildHeaders(){
			this->_response.append("Connection: close\r\n"); 								//MANDATORY
			this->_response.append("Date: " + getActualTime() + "\r\n"); 					//OPTIONNAL
			this->_response.append("Server: 42webserv\r\n"); 								//OPTIONNAL
			this->_response.append("Content-Length: " + itos(_content.length()) + "\r\n");	//The connection keep-alive if not present
		}

		void _buildBody(){
			this->_response.append("\r\n");

			//this->_response.append("Response sent by 42 webserv\r\n");
			this->_response.append(_content + "\r\n");
		}

		//Status
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

		httpRequest						_request;
		std::map<int, std::string> 		_statusMessage;

		std::string						_content;

		std::string						_response;
};//end of class httpResponse

#endif