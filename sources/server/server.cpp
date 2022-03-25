/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:14:35 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/23 20:02:45 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"
#include "Config.hpp"
#include "server.hpp"
#include "system.hpp"

bool	g_running = true;

static void	initServer(std::vector<EpollSocket> &localServers, Epoll &epoll)
{
	signal(SIGINT, handleSigint);
	for (size_t i = 0; i < localServers.size(); ++i)
	{
		localServers[i].events(EPOLLIN | EPOLLET | EPOLLRDHUP);
		epoll.control(EPOLL_CTL_ADD, localServers[i]);
	}
}

void	server(std::vector<EpollSocket> &localServers, Config &config)
{
	int			nfds;
	Epoll		epoll;
	EpollSocket	socketEvent;

	initServer(localServers, epoll);
	while (g_running == true)
	{
		nfds = epoll.wait();
		for (int n = 0; n < nfds; ++n)
		{
			socketEvent = epoll.socketAt(n);
			if (std::find(localServers.begin(), localServers.end(), socketEvent) != localServers.end())
				handleConnection(epoll, socketEvent);
			else if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
				handleDeconnection(epoll, socketEvent);
			else if (socketEvent.events() & EPOLLIN)
				handleInput(socketEvent, config);
		}
	}
}
