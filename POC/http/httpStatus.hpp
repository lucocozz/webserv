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

std::string					getStatusMessage(int statusCode){
	//SUCCESS
	if (statusCode == OK)
		return ("OK");
	if (statusCode == CREATED)
		return ("Created");
	//ERROR CLIENT
	if (statusCode == BAD_REQUEST)
		return ("Bad Request");
	if (statusCode == NOT_FOUND)
		return ("Not Found");
	if (statusCode == LENGTH_REQUIRED)
		return ("Length Required");
	//ERROR SERVER
	if (statusCode == INTERNAL_SERVER_ERROR)
		return ("Internal Server Error");
	if (statusCode == NOT_IMPLEMENTED)
		return ("Not Implemented");
	if (statusCode == SERVICE_UNAVAILABLE)
		return ("Service Unavailable");
	if (statusCode == HTTP_VERSION_NOT_SUPPORTED)
		return ("http Version Not Supported");
	return ("OK");
}

#endif