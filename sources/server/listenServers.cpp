/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   listenServers.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 19:58:50 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/23 19:59:43 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <iostream>
#include "EpollSocket.hpp"
#include "Config.hpp"

std::vector<EpollSocket>	listenServers(Config &config)
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
		serverSocket.setNonBlocking();
		serverSocket.setSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
		serverSocket.bindSocket();
		serverSocket.listenSocket();
		LocalServers.push_back(serverSocket);
		std::cout << std::endl;
	}
	return (LocalServers);
}
