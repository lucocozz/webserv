/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:48:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/23 21:34:19 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "Config.hpp"
#include "EpollSocket.hpp"
#include "server.hpp"

int	main(void)
{
	Config						config;
	std::vector<EpollSocket>	localServers;

	try {
		config.parse(std::string(WEBSERV_PATH) + "webserv.conf");
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

