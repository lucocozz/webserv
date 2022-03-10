/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:57 by user42            #+#    #+#             */
/*   Updated: 2022/03/10 15:59:04 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSCODE_HPP
#define STATUSCODE_HPP

#include <string>

enum STATUS_CODE{
	//Information
	CONTINUE = 100,
	PROCESSING = 102,
	//Success
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	//Redirection
	MULTIPLE_CHOICES = 300,
	NOT_MODIFIED = 304,
	//Error client
	BAD_REQUEST = 400,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	LENGTH_REQUIRED = 411,
	REQUEST_ENTITY_TOO_LARGE = 413,
	//Error server
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	SERVICE_UNAVAILABLE = 503,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

std::map<int, std::string>	initStatusMessages(){
	std::map<int, std::string> ret;

	//Information
	ret.insert(std::make_pair(CONTINUE, "Continue"));
	ret.insert(std::make_pair(PROCESSING, "Processing"));
	//Success
	ret.insert(std::make_pair(OK, "OK"));
	ret.insert(std::make_pair(CREATED, "Created"));
	ret.insert(std::make_pair(ACCEPTED, "Accepted"));
	ret.insert(std::make_pair(NO_CONTENT, "No_Content"));
	ret.insert(std::make_pair(RESET_CONTENT, "Reset_Content"));
	ret.insert(std::make_pair(PARTIAL_CONTENT, "Partial_Content"));
	//Redirection
	ret.insert(std::make_pair(MULTIPLE_CHOICES, "Multiple Choices"));
	ret.insert(std::make_pair(NOT_MODIFIED, "Not Modified"));
	//Error client
	ret.insert(std::make_pair(BAD_REQUEST, "Bad Request"));
	ret.insert(std::make_pair(FORBIDDEN, "Forbidden"));
	ret.insert(std::make_pair(NOT_FOUND, "Not Found"));
	ret.insert(std::make_pair(METHOD_NOT_ALLOWED, "Method Not Allowed"));
	ret.insert(std::make_pair(LENGTH_REQUIRED, "Length Required"));
	ret.insert(std::make_pair(REQUEST_ENTITY_TOO_LARGE, "Request Entity Too Large"));
	//Error server
	ret.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
	ret.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
	ret.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
	ret.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "http Version Not Supported"));

	return (ret);
}

std::map<int, std::string>	statusMessages = initStatusMessages();

#endif