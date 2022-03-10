/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/21 19:48:40 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/10 01:20:35 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <exception>
#include <iostream>
#include "Config.hpp"

int	main(void)
{
	Config	config;

	try {
		config.parse(std::string(WEBSERV_PATH) + "config/default.conf");
		std::cout << config.servers[0].directives.at("root")[0] << std::endl;
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}

