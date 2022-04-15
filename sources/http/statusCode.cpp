/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statusCode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:36 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 20:19:12 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "statusCode.hpp"

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
	ret.insert(std::make_pair(UNPROCESSABLE_ENTITY, "Unprocessable Entity"));
	//Error server
	ret.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
	ret.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
	ret.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
	ret.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "http Version Not Supported"));

	return (ret);
}