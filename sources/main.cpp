/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:48:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/30 08:30:39 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include <syslog.h>
#include "Config.hpp"
#include "EpollSocket.hpp"
#include "server.hpp"

#ifndef CONFIG_FILE_NAME
# define CONFIG_FILE_NAME "webserv.conf"
#endif

int	main(void)
{
	Config						config;
	std::vector<EpollSocket>	localServers;

	try {
		config.parse(std::string(WEBSERV_PATH) + CONFIG_FILE_NAME);
		localServers = listenServers(config);
		server(localServers, config);
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	for (size_t i = 0; i < localServers.size(); ++i)
		localServers[i].closeSocket();
	return (0);
}

