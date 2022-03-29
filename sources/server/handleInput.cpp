/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/29 14:25:22 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "request.hpp"
#include "response.hpp"

void	handleInput(EpollSocket &client, const Config &serverConfig)
{
	std::pair<std::string, int>					data;
	const std::pair<std::string, std::string> 	clientInfo(client.getNameInfo(NI_NUMERICHOST), client.getNameInfo());
	httpRequest 								request;
	httpResponse								response;

	data = client.recvData();
	std::cout << data.first << std::endl;
	request.treatRequest(data.first, serverConfig);
	response.buildResponse(request, serverConfig, clientInfo);
	response.sendResponse(client);
}
