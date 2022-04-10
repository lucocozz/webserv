/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eventLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 16:50:53 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/09 23:07:57 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <vector>
#include <iostream>
#include "EpollSocket.hpp"
#include "Server.hpp"
#include "Client.hpp"
#include "Epoll.hpp"
#include "serverCore.hpp"

static bool	isAServer(std::vector<Server> &serverList, EpollSocket &socketEvent)
{
	for (size_t i = 0; i < serverList.size(); ++i)
	{
		if (serverList[i].socket.listener() == socketEvent.listener())
			return (true);
	}
	return (false);
}

void	eventLoop(std::vector<Server> &serverList, Epoll &epoll, int events)
{
	EpollSocket				socketEvent;
	std::map<int, Client>	clientList;

	for (int n = 0; n < events; ++n)
	{
		socketEvent = epoll.socketAt(n);
		if (isAServer(serverList, socketEvent) == true)
			handleConnection(serverList, clientList, socketEvent, epoll);
		else if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
			handleDeconnection(clientList, socketEvent, epoll);
		else if (socketEvent.events() & EPOLLIN)
			handleInput(clientList[socketEvent.listener()]);
		// else if (socketEvent.events() & EPOLLOUT)
			// handleOutput(*server, socketEvent);
	}
}
