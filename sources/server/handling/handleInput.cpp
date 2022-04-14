/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/15 00:44:31 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "response.hpp"
#include "Server.hpp"
#include "response.hpp"
#include "Client.hpp"

void	handleInput(Client &client)
{
	std::pair<std::string, int>					data;
	std::string 								hostName("");
	Server										*serverLink;
	const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	httpRequest 								request;
	httpResponse								response;

	data = client.socket.recvData();
	std::cout << "client datas: |" << data.first << "|" << std::endl;
	if (data.first.find("Host") != std::string::npos){
		std::string::iterator 	itb;
		std::string::iterator	ite;

		itb = data.first.begin() + data.first.find("Host:") + 6;
		ite = itb;
		while (*ite != '\n' && *ite != ':')
			ite++;
		hostName.append(itb, ite);
	}
	serverLink = client.getServerLinks(hostName);
	//client.request.treatRequest(data.first, *serverLink);
	//client.response.buildResponse(client.request, *serverLink, clientInfo);
	//client.response.sendResponse(client.socket);
	request.treatRequest(data.first, *serverLink);
	response.buildResponse(request, *serverLink, clientInfo);
	response.sendResponse(client.socket);
}
