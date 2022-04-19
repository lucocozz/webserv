/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/18 22:35:51 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

static size_t	getContentLenght(std::string &request){
	std::string::iterator 	itb = request.begin() + request.find("Content-Length:") + 16;
	std::string::iterator 	ite = itb;
	std::string				contentLengthStr("");

	while (isdigit(*ite))
		ite++;
	contentLengthStr.append(itb, ite);
	return (atoi(contentLengthStr.c_str()));
}

void	handleInput(Client &client)
{
	std::pair<std::string, int>					data;
	static std::string							rawRequest("");
	static size_t								contentLength = 0;
	static bool									chunked = false;
	std::string 								hostName("");
	Server										*serverLink;
	const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	httpRequest 								request;
	httpResponse								response;
	
	data = client.socket.recvData();
	//std::cout << "client datas: |" << data.first << "|" << std::endl;

	//check if the request is POST, if it is I set contentLength
	if (data.first.find("POST") != std::string::npos){
		if (data.first.find("Transfer-Encoding: chunked") != std::string::npos)
			chunked = true;
		else
			contentLength = getContentLenght(data.first);
	}
	else if (data.first.find("GET") != std::string::npos)
		contentLength = data.second;
	
	//While the concatened request is smaller than the header content-length I keep concatenating || if the chunked bool is still true
	if (rawRequest.size() < contentLength || chunked == true)
		rawRequest.append(data.first);
	//If there is 0\r\n at the end of a chunked request chunked bool is set to false which means that the request has been fully received
	if (rawRequest.find("0\r\n", rawRequest.size() - 5) != std::string::npos && chunked == true)
		chunked = false;
	//If the request has been fully received we treat it and build the response then set all the statics to default
	if (rawRequest.size() >= contentLength && chunked == false){
		serverLink = client.getServerLinks(hostName);
		request.treatRequest(rawRequest, *serverLink);
		response.buildResponse(request, *serverLink, clientInfo);
		response.sendResponse(client.socket);
		rawRequest.clear();
		contentLength = 0;
		chunked = false;
	}
}
