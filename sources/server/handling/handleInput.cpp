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

void	handleInput(Client &client)
{
	std::pair<std::string, int>					data;
	std::string 								hostName("");
	Server										*serverLink;
	const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	httpRequest 								request;
	httpResponse								response;
	
	data = client.socket.recvData();
	//std::cout << "client datas: |" << data.first << "|" << std::endl;
	serverLink = client.getServerLinks(hostName);
	request.treatRequest(data.first, *serverLink);
	response.buildResponse(request, *serverLink, clientInfo);
	response.sendResponse(client.socket);
}
