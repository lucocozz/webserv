/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/31 00:42:36 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "request.hpp"
#include "Server.hpp"
#include "response.hpp"

void	handleInput(Server &server, EpollSocket &socketEvent)
{
	std::pair<std::string, int>					data;
	const std::pair<std::string, std::string> 	clientInfo(socketEvent.getNameInfo(NI_NUMERICHOST), socketEvent.getNameInfo());
	httpRequest 								request;
	httpResponse								response;

	data = socketEvent.recvData();
	std::cout << "client datas: |" << data.first << "|" << std::endl;
	request.treatRequest(data.first, server);
	response.buildResponse(request, server, clientInfo);
	response.sendResponse(socketEvent);
}
