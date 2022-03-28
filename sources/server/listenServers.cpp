/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listenServers.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 19:58:50 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/28 22:40:15 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <iostream>
#include "EpollSocket.hpp"
#include "Config.hpp"

static EpollSocket	createServer(std::string port)
{
	EpollSocket	server;

	server.createSocket(AF_INET, SOCK_STREAM, port);
	server.setNonBlocking();
	server.setSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
	server.bindSocket();
	server.listenSocket();
	return (server);
}

static bool	checkServersPort(std::vector<EpollSocket> &servers, std::string port)
{
	for (size_t i = 0; i < servers.size(); ++i)
	{
		if (servers[i].port() == port)
			return (true);
	}
	return (false);
}

std::vector<EpollSocket>	listenServers(Config &config)
{
	std::vector<EpollSocket>	LocalServers;
	std::vector<std::string>	listen;
	std::string					serverName;

	for (size_t i = 0; i < config.servers.size(); ++i)
	{
		listen = config.servers[i].directives.at("listen");
		serverName = config.servers[i].directives.at("server_name")[0];
		std::cout << "Creating server: " << serverName << std::endl;
		std::cout << "listening on " << listen[0] << ":" << listen[1] << std::endl;
		try {
			if (checkServersPort(LocalServers, listen[1]) == false)
				LocalServers.push_back(createServer(listen[1]));
		} catch (const std::exception &e) {
			for (size_t j = 0; j < LocalServers.size(); ++j)
				LocalServers[j].closeSocket();
			throw (std::runtime_error(e.what()));
		}
		std::cout << std::endl;
	}
	return (LocalServers);
}
