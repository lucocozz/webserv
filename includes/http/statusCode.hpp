/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statusCode.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:58:57 by user42            #+#    #+#             */
/*   Updated: 2022/04/29 18:04:37 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STATUSCODE_HPP
#define STATUSCODE_HPP


//#include "stringRelated.hpp"
#include <map>
#include <string>

enum STATUS_CODE{
	//Information
	CONTINUE = 100,
	SWITCHING_PROTOCOL = 101,
	PROCESSING = 102,
	EARLY_HINTS = 103,

	//Success
	OK = 200,
	CREATED = 201,
	ACCEPTED = 202,
	NON_AUTHORITATIVE_INFORMATION = 203,
	NO_CONTENT = 204,
	RESET_CONTENT = 205,
	PARTIAL_CONTENT = 206,
	MULTI_STATUS = 207,
	ALREADY_STATUS = 208,
	CONTENT_DIFFERENT = 210,
	IM_USED = 226,

	//Redirection
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY = 301,
	FOUND = 302,
	SEE_OTHER = 303,
	NOT_MODIFIED = 304,
	USE_PROXY = 305,
	SWITCH_PROXY = 306,
	TEMPORARY_REDIRECT = 307,
	PERMANENT_REDIRECT = 308,
	TOO_MANY_REDIRECTS = 310,
	
	//Error client
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	PAYMENT_REQUIRED = 402,
	FORBIDDEN = 403,
	NOT_FOUND = 404,
	METHOD_NOT_ALLOWED = 405,
	NOT_ACCEPTABLE = 406,
	PROXY_AUTHENTIFICATION_REQUIRED = 407,
	REQUEST_TIME_OUT = 408,
	CONFLICT = 409,
	GONE = 410,
	LENGTH_REQUIRED = 411,
	PAYLOAD_TOO_LARGE = 413,
	REQUEST_URL_TOO_LONG = 414,
	UNSUPPORTED_MEDIA_TYPE = 415,
	REQUESTED_RANGE_INSATISFIABLE = 416,
	EXCEPTATION_FAILED = 417,
	IM_A_TEA_POT = 418,
	BAD_MAPPING_MISDIRECTED_REQUEST = 421,
	UNPROCESSABLE_ENTITY = 422,
	LOCKED = 423,
	METHOD_FAILURE = 424,
	TOO_EARLY = 425,
	UPGRADE_REQUIRED = 426,
	PRECONDITION_REQUIRED = 428, 
	TOO_MANY_REQUESTS = 429,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	RETRY_WITH = 449, 
	BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS = 450,
	UNAVAILABLE_FOR_LEGAL_REASONS = 451,
	UNRECOVERABLE_ERROR = 456,
	
	//Error server
	INTERNAL_SERVER_ERROR = 500,
	NOT_IMPLEMENTED = 501,
	BAD_GATEWAY = 502,
	SERVICE_UNAVAILABLE = 503,
	GATEWAY_TIME_OUT = 504,
	HTTP_VERSION_NOT_SUPPORTED = 505,
	VARIANT_ALSO_NEGOTIATES = 506,
	INSUFFICIENT_STORAGE = 507,
	LOOP_DETECTED = 508,
	BANDWIDTH_LIMIT_EXCEEDED = 509,
	NOT_EXTENDED = 510,
	NETWORK_AUTHENTICATION_REQUIRED = 511
};

std::map<int, std::string>		initStatusMessages();

#endif
