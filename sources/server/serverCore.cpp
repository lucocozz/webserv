/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:14:35 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/31 00:44:04 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"
#include "Config.hpp"
#include "serverCore.hpp"
#include "system.hpp"

bool	g_running = true;

static void	initServer(std::vector<Server> &localServers, Epoll &epoll)
{
	signal(SIGINT, handleSigint);
	for (size_t i = 0; i < localServers.size(); ++i)
	{
		localServers[i].socket.events(EPOLLIN | EPOLLET | EPOLLRDHUP);
		epoll.control(EPOLL_CTL_ADD, localServers[i].socket);
	}
}

static Server	*findServerSocket(std::vector<Server> &localServers, EpollSocket &socketEvent)
{
	for (size_t i = 0; i < localServers.size(); ++i)
	{
		if (localServers[i].socket.listener() == socketEvent.listener())
			return (&localServers[i]);
	}
	return (NULL);
}


static	Server	*findServerFromClient(std::vector<Server> &localServers, EpollSocket &socketEvent)
{	
	for (size_t i = 0; i < localServers.size(); ++i)
	{
		for (size_t j = 0; j < localServers[i].clients.size(); ++j)
		{
			if (localServers[i].clients[j].listener() == socketEvent.listener())
				return (&localServers[i]);
		}
	}
	return (NULL);
}

void	serverCore(std::vector<Server> &localServers)
{
	int			nfds;
	Epoll		epoll;
	Server		*server;
	EpollSocket	socketEvent;

	initServer(localServers, epoll);
	while (g_running == true)
	{
		nfds = epoll.wait();
		for (int n = 0; n < nfds; ++n)
		{
			socketEvent = epoll.socketAt(n);
			server = findServerSocket(localServers, socketEvent);
			if (server != NULL)
				handleConnection(*server, socketEvent, epoll);
			else
			{
				server = findServerFromClient(localServers, socketEvent);
				if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
					handleDeconnection(*server, socketEvent, epoll);
				else if (socketEvent.events() & EPOLLIN)
					handleInput(*server, socketEvent);
			}
		}
	}
}
