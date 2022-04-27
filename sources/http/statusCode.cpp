/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   statusCode.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:36 by user42            #+#    #+#             */
/*   Updated: 2022/04/27 17:14:02 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "statusCode.hpp"


std::map<int, std::string>	initStatusMessages(){
	std::map<int, std::string> ret;

	//Information 1xx
	ret.insert(std::make_pair(CONTINUE, "Continue"));
	ret.insert(std::make_pair(PROCESSING, "Processing"));
	ret.insert(std::make_pair(SWITCHING_PROTOCOL, "Switching Protocol"));
	ret.insert(std::make_pair(EARLY_HINTS, "Early Hints"));

	//Success 2xx
	ret.insert(std::make_pair(OK, "OK"));
	ret.insert(std::make_pair(CREATED, "Created"));
	ret.insert(std::make_pair(ACCEPTED, "Accepted"));
	ret.insert(std::make_pair(NON_AUTHORITATIVE_INFORMATION, "Non Authoritative Information"));
	ret.insert(std::make_pair(NO_CONTENT, "No_Content"));
	ret.insert(std::make_pair(RESET_CONTENT, "Reset_Content"));
	ret.insert(std::make_pair(PARTIAL_CONTENT, "Partial_Content"));
	ret.insert(std::make_pair(MULTI_STATUS, "Multi Status"));
	ret.insert(std::make_pair(ALREADY_STATUS, "Already Status"));
	ret.insert(std::make_pair(CONTENT_DIFFERENT, "Content Different"));
	ret.insert(std::make_pair(IM_USED, "IM Used"));

	//Redirection 3xx
	ret.insert(std::make_pair(MULTIPLE_CHOICES, "Multiple Choices"));
	ret.insert(std::make_pair(MOVED_PERMANENTLY, ""));
	ret.insert(std::make_pair(FOUND, "Found"));
	ret.insert(std::make_pair(SEE_OTHER, "See Other"));
	ret.insert(std::make_pair(NOT_MODIFIED, "Not Modified"));
	ret.insert(std::make_pair(USE_PROXY, "Use Proxy"));
	ret.insert(std::make_pair(SWITCH_PROXY, "Switch Proxy"));
	ret.insert(std::make_pair(TEMPORARY_REDIRECT, "Temporary Redirect"));
	ret.insert(std::make_pair(PERMANENT_REDIRECT, "Permanent Redirect"));
	ret.insert(std::make_pair(TOO_MANY_REDIRECTS, "Too Many Redirects"));

	//Error client 4xx
	ret.insert(std::make_pair(BAD_REQUEST, "Bad Request"));
	ret.insert(std::make_pair(UNAUTHORIZED, "Unauthorized"));
	ret.insert(std::make_pair(PAYMENT_REQUIRED, "Payment Required"));
	ret.insert(std::make_pair(FORBIDDEN, "Forbidden"));
	ret.insert(std::make_pair(NOT_FOUND, "Not Found"));
	ret.insert(std::make_pair(METHOD_NOT_ALLOWED, "Method Not Allowed"));
	ret.insert(std::make_pair(NOT_ACCEPTABLE, "Not Acceptable"));
	ret.insert(std::make_pair(PROXY_AUTHENTIFICATION_REQUIRED, "Proxy Authentification Required"));
	ret.insert(std::make_pair(REQUEST_TIME_OUT, "Request Time Out"));
	ret.insert(std::make_pair(CONFLICT, "Conflict"));
	ret.insert(std::make_pair(GONE, "Gone"));
	ret.insert(std::make_pair(LENGTH_REQUIRED, "Length Required"));
	ret.insert(std::make_pair(PAYLOAD_TOO_LARGE, "Payload Too Large"));
	ret.insert(std::make_pair(REQUEST_URL_TOO_LONG, "Request Url Too Long"));
	ret.insert(std::make_pair(UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Types"));
	ret.insert(std::make_pair(REQUESTED_RANGE_INSATISFIABLE, "Requested Range Insatisfiable"));
	ret.insert(std::make_pair(EXCEPTATION_FAILED, "Exceptation Failed"));
	ret.insert(std::make_pair(IM_A_TEA_POT, "Im a tea pot"));
	ret.insert(std::make_pair(BAD_MAPPING_MISDIRECTED_REQUEST, "Bad mapping / Misdirected Request"));
	ret.insert(std::make_pair(UNPROCESSABLE_ENTITY, "Unprocessable Entity"));
	ret.insert(std::make_pair(LOCKED, "Locked"));
	ret.insert(std::make_pair(METHOD_FAILURE, "Method Failure"));
	ret.insert(std::make_pair(TOO_EARLY, "Too Early"));
	ret.insert(std::make_pair(UPGRADE_REQUIRED, "Upgrade Required"));
	ret.insert(std::make_pair(PRECONDITION_REQUIRED, "Precondition Required"));
	ret.insert(std::make_pair(TOO_MANY_REQUESTS, "Too Many Requests"));
	ret.insert(std::make_pair(REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large"));
	ret.insert(std::make_pair(RETRY_WITH, "Retry With"));
	ret.insert(std::make_pair(BLOCKED_BY_WINDOWS_PARENTAL_CONTROLS, "Blocked By Windows Parent Controls"));
	ret.insert(std::make_pair(UNAVAILABLE_FOR_LEGAL_REASONS, "Unavailable For Legal Reasons"));
	ret.insert(std::make_pair(UNRECOVERABLE_ERROR, "Unrecoverable Error"));

	//Error server 5xx
	ret.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
	ret.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
	ret.insert(std::make_pair(BAD_GATEWAY, "Bad Gateway"));
	ret.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
	ret.insert(std::make_pair(GATEWAY_TIME_OUT, "Gateway Time Out"));
	ret.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "http Version Not Supported"));
	ret.insert(std::make_pair(VARIANT_ALSO_NEGOTIATES, "Variant Also Negotiates"));
	ret.insert(std::make_pair(INSUFFICIENT_STORAGE, "Insufficient Storage"));
	ret.insert(std::make_pair(LOOP_DETECTED, "Loop Detected"));
	ret.insert(std::make_pair(BANDWIDTH_LIMIT_EXCEEDED, "Bandwidth Limit Exceeded"));
	ret.insert(std::make_pair(NOT_EXTENDED, "Not Extended"));
	ret.insert(std::make_pair(NETWORK_AUTHENTICATION_REQUIRED, "Network Authentification Required"));

	return (ret);
}
