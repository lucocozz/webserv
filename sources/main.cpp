/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:48:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/14 17:17:21 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include <syslog.h>
#include "Config.hpp"
#include "EpollSocket.hpp"
#include "Server.hpp"
#include "serverCore.hpp"

#ifndef CONFIG_FILE_NAME
# define CONFIG_FILE_NAME "webserv.conf"
#endif

int	main(void)
{
	Config				config;
	std::vector<Server>	serverList;

	try {
		config.parse(std::string(WEBSERV_PATH) + CONFIG_FILE_NAME);
		serverList = createServers(config);
		serverCore(serverList);
	}
	catch (const std::exception &e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
	closeServers(serverList);
	return (0);
}

