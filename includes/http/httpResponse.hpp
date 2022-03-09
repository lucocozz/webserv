#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "httpRequest.hpp"
#include "../socket/Epoll.hpp"

class httpResponse{
	public:
		httpResponse(){
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
			this->_retrieveContent();
			this->_buildStatusLine();
			this->_buildHeaders();
			this->_buildBody();
			std::cout << std::endl << "SERVER RESPONSE :" << std::endl;
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
			//Mandatory HEADER (even if error)
			this->_response.append("Server: 42webserv/0.0.1\r\n");
			this->_response.append("Date: " + getActualTime() + "\r\n");
			this->_response.append("Content-Type: " + _contentType + "\r\n"); //Need a functino to find the content-type
			this->_response.append("Content-Length: " + itos(_content.length()) + "\r\n");
			this->_response.append("Connection: close\r\n"); //close or keep-alive make an enum
			//OPTIONNAL HEADERS (depending on method used if no error occurs)
			if (this->_request.getStatus() / 100 == 2){
				this->_response.append("Last-Modified: " + getFileModification(this->_request.getPath().c_str()) + "\r\n");
				this->_response.append("Etag: " + makeETag(this->_request.getPath().c_str()) + "\r\n");
				this->_response.append("Accept-Ranges: bytes\r\n"); //Can be none but useless, only bytes ranges is defined by RFC
			}
		}

		void _buildBody(){
			this->_response.append("\r\n");
			this->_response.append(_content + "\r\n");
		}

		//Retrive the content
		void	_retrieveContent(){
			if (this->_request.getStatus() / 100 != 2){
				_contentType = "text/html";
				std::string title = itos(this->_request.getStatus()) + " " + _getStatusMessage();
				_content.append("<html>\n<head><title>" + title + "</title></head>\n");
				_content.append("<body>\n<center><h1>" + title + "</h1></center>\n");
				_content.append("<hr><center>42webserv/0.0.1</center>\n");
				_content.append("</body>\n</html>");
				return;
			}
			//Need to retrieve the content if allowed and identify his type
			_content.append("<html>\n<head><title>Welcome to 42webserv</title></head>\n");
			_content.append("<center><h3>" + itos(this->_request.getStatus()) + " " + _getStatusMessage() + "</h3></center>");
			_content.append("<body>\n<center><h1>Welcome to 42webserv !</h1></center>\n");
			_content.append("<p><center>If you see this page, the 42webserv is successfully installed and working. Further configuration is required.</center></p>\n");
			_content.append("<hr><center>42webserv/0.0.1</center>\n");
			_content.append("</body>\n</html>");
			_contentType = "text/html";
		}

		std::string	_getStatusMessage(){
			return ((*statusMessages.find(this->_request.getStatus())).second);
		}

		httpRequest						_request;

		std::string						_content;
		std::string						_contentType;

		std::string						_response;
};//end of class httpResponse

#endif