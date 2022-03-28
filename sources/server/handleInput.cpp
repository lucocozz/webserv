/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/28 23:36:31 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "request.hpp"
#include "response.hpp"

static std::pair<bool,LocationContext>  getLocation(std::string path, std::vector<LocationContext> serverLocation) {
	std::pair<bool,LocationContext> locationPair = std::make_pair(true, serverLocation[0]);

	for (size_t i = 0; i < serverLocation.size(); i++) {
		if (path.find(serverLocation[i].args[0]) != std::string::npos) {
			locationPair.second = serverLocation[i];
			return(locationPair);
		}
	}
	locationPair.first = false;
	return (locationPair);
}

void	handleInput(EpollSocket &client, const Config &serverConfig)
{
	std::pair<std::string, int>					data;
	const std::pair<std::string, std::string> 	clientInfo(client.getNameInfo(NI_NUMERICHOST), client.getNameInfo());
	httpRequest 								request;
	httpResponse								response;

	data = client.recvData();
	request.treatRequest(data.first, serverConfig);
	response.buildResponse(request, serverConfig, clientInfo);
	response.sendResponse(client);
}
