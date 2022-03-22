/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:48:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/22 19:42:13 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "Config.hpp"
#include "EpollSocket.hpp"
#include "server.hpp"

static std::vector<EpollSocket>	listenServers(Config &config)
{
	EpollSocket					serverSocket;
	std::vector<EpollSocket>	LocalServers;
	std::vector<std::string>	listen;

	for (size_t i = 0; i < config.servers.size(); ++i)
	{
		serverSocket = EpollSocket();
		listen = config.servers[i].directives.at("listen");
		std::cout << "Creating server: " << config.servers[i].directives.at("server_name")[0] << std::endl;
		std::cout << "listening " << listen[0] << ":" << listen[1] << std::endl;
		serverSocket.createSocket(AF_INET, SOCK_STREAM, listen[1]);
		serverSocket.setSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
		serverSocket.bindSocket();
		serverSocket.listenSocket();
		LocalServers.push_back(serverSocket);
		std::cout << std::endl;
	}
	return (LocalServers);
}

int	main(void)
{
	Config						config;
	std::vector<EpollSocket>	localServers;

	try {
		config.parse(std::string(WEBSERV_PATH) + "sites_availables/default.conf");
		localServers = listenServers(config);
		server(localServers, config);
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	for (size_t i = 0; i < localServers.size(); ++i)
		localServers[i].closeSocket();
	return (0);
}

