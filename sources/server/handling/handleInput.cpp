/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/11 19:37:40 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "request.hpp"
#include "Server.hpp"
#include "response.hpp"
#include "Client.hpp"

void	handleInput(Client &client)
{
	std::pair<std::string, int>					data;
	const std::pair<std::string, std::string> 	clientInfo(client.getSocket().getNameInfo(NI_NUMERICHOST), client.getSocket().getNameInfo());
	httpRequest 								request;
	httpResponse								response;

	data = client.getSocket().recvData();
	std::cout << "client datas: |" << data.first << "|" << std::endl;
	request.treatRequest(data.first, *(client.getServerLinks().at(0)));
	response.buildResponse(request, *(client.getServerLinks().at(0)), clientInfo);
	response.sendResponse(client.getSocket());
}
