/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/30 15:43:25 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/30 19:09:03 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "EpollSocket.hpp"
#include "Config.hpp"

class Server
{
public:
	EpollSocket					socket;
	ServerContext				context;
	std::vector<EpollSocket>	clients;

	Server(void) {}

	Server(EpollSocket &socket, ServerContext &context) : socket(socket), context(context) {}

	Server(const Server &rhs)
	{
		*this = rhs;
	}

	~Server(void) {}

	Server	&operator=(const Server &rhs)
	{
		if (this != &rhs)
		{
			this->socket = rhs.socket;
			this->context = rhs.context;
		}
		return (*this);
	}
};

