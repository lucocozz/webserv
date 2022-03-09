#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <string>

enum STATUS_CODE{
	//Success
	OK = 200,
	CREATED = 201,
	//Error client
	BAD_REQUEST = 400,
	NOT_FOUND = 404,
	LENGTH_REQUIRED = 411,
	//Error server
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	SERVICE_UNAVAILABLE = 503,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

std::map<int, std::string>	initStatusMessages(){
	std::map<int, std::string> ret;

	//Success
	ret.insert(std::make_pair(OK, "OK"));
	ret.insert(std::make_pair(CREATED, "Created"));
	//Error client
	ret.insert(std::make_pair(BAD_REQUEST, "Bad Request"));
	ret.insert(std::make_pair(NOT_FOUND, "Not Found"));
	ret.insert(std::make_pair(LENGTH_REQUIRED, "Length Required"));
	//Error server
	ret.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
	ret.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
	ret.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
	ret.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "http Version Not Supported"));

	return (ret);
}

std::map<int, std::string>	statusMessages = initStatusMessages();


void	setStatusMessage(std::map<int, std::string> &statusMessage){
	statusMessage.insert(std::make_pair(OK, "OK"));
	statusMessage.insert(std::make_pair(CREATED, "Created"));
	statusMessage.insert(std::make_pair(BAD_REQUEST, "Bad Request"));
	statusMessage.insert(std::make_pair(NOT_FOUND, "Not Found"));
	statusMessage.insert(std::make_pair(LENGTH_REQUIRED, "Length Required"));
	statusMessage.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
	statusMessage.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
	statusMessage.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
	statusMessage.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "http Version Not Supported"));
}

#endif