#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "httpRequest.hpp"

class httpResponse{
	public:
		httpResponse(){
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

		//Answering
		void	_answer(){
			this->_buildStatusLine();
		}

		void _buildStatusLine(){
			this->_response.append("HTTP/1.1 ");
			this->_response.append(itos(this->_request.getStatus()) + " ");
			this->_response.append(getStatusMessage(this->_request.getStatus()) + "\r\n");
		}

		httpRequest					_request;
		std::string					_response;
};//end of class httpResponse

#endif