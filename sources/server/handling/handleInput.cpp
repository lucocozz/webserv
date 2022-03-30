/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/30 19:33:51 by lucocozz         ###   ########.fr       */
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

	(void)server;
	data = socketEvent.recvData();
	std::cout << data.first << std::endl;
	// request.treatRequest(data.first, server.context);
	// response.buildResponse(request, server.context, clientInfo);
	// response.sendResponse(socketEvent);
}
