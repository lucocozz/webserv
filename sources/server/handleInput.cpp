/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/22 21:56:16 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"
#include "Config.hpp"
#include "CGI.hpp"
#include "request.hpp"
#include "response.hpp"

static bool checkExtension(const std::string &filePath, const std::string &path, const std::string &extension){
	//filePath should be the args[0] of the location 
	std::string	scriptExtension;
	size_t 		begin;
	size_t 		end;
	size_t		cnt;

	begin = path.find('.', path.find(filePath, 0) + filePath.size());
	end = begin + 1;
	cnt = end;
	while (isalpha(path[cnt]))
		cnt++;
	end = cnt - end + 1;
	scriptExtension = path.substr(begin, end);
	if (extension.size() == scriptExtension.size() &&
		extension.compare(scriptExtension) == 0)
		return (true);
	return (false);
}

static std::pair<bool,LocationContext>  getLocation(std::string path, std::vector<LocationContext> serverLocation){
	std::pair<bool,LocationContext> locationPair = std::make_pair(true, serverLocation[0]);

	for (size_t i = 0; i < serverLocation.size(); i++){
		if (path.find(serverLocation[i].directives.at("cgi_extension")[0]) != std::string::npos &&
			checkExtension("/cgiphp/", path, serverLocation[i].directives.at("cgi_extension")[0]) == true){
			return(locationPair);
		}
		locationPair.second = serverLocation[i];
	}
	locationPair.first = false;
	return (locationPair);
}

void	handleInput(EpollSocket &client, const Config &serverConfig)
{
	std::pair<std::string, int>					data;
	const std::pair<std::string, std::string> 	clientInfo(client.getNameInfo(client.listener(), NI_NUMERICHOST), client.getNameInfo(client.listener()));
	httpRequest 								request;
	httpResponse								response;

	data = client.recvData();
	request.treatRequest(data.first, serverConfig);
	//serverConfig.servers[0] ->  will change depending on what server we work on
	std::pair<bool,LocationContext> locationResult = getLocation(request.getPath(), serverConfig.servers[0].locations);
	if (locationResult.first != false){
		try{
			//serverConfig.servers[0] ->  will change depending on what server we work on
			std::pair<ServerContext, LocationContext > serverLocation = 
			std::make_pair(serverConfig.servers[0], locationResult.second);

			CGI cgi(data, request.getHeaders(), serverLocation, clientInfo);
			cgi.CGIStartup();
		}
		catch(const std::exception &e){
			std::cout << e.what() << std::endl;
		}
	}
	response.buildResponse(request, serverConfig);
	response.sendResponse(client);
}
