/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:57 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 20:19:12 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSCODE_HPP
#define STATUSCODE_HPP

#include "stringRelated.hpp"
#include <map>

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
	UNPROCESSABLE_ENTITY = 422,
	//Error server
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	SERVICE_UNAVAILABLE = 503,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

std::map<int, std::string>	initStatusMessages();

#endif
