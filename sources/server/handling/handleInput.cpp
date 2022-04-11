/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/11 22:06:54 by lucocozz         ###   ########.fr       */
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
	const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	httpRequest 								request;
	httpResponse								response;

	data = client.socket.recvData();
	std::cout << "client datas: |" << data.first << "|" << std::endl;
	request.treatRequest(data.first, *(client.getServerLinks().at(0)));
	response.buildResponse(request, *(client.getServerLinks().at(0)), clientInfo);
	response.sendResponse(client.socket);
}
