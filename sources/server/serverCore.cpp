/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:14:35 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/11 21:58:32 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"
#include "Config.hpp"
#include "serverCore.hpp"
#include "system.hpp"
#include "Client.hpp"

bool	g_running = true;

static void	initServer(std::vector<Server> &serverList, Epoll &epoll)
{
	EpollSocket			socket;
	std::vector<int>	controledSocket;

	signal(SIGINT, handleSigint);
	for (size_t i = 0; i < serverList.size(); ++i)
	{
		socket = serverList[i].socket;
		if (std::find(controledSocket.begin(), controledSocket.end(), socket.listener()) == controledSocket.end())
		{
			socket.events(EPOLLIN | EPOLLET | EPOLLOUT | EPOLLRDHUP);
			epoll.control(EPOLL_CTL_ADD, socket);
			controledSocket.push_back(socket.listener());
		}
	}
}

void	serverCore(std::vector<Server> &serverList)
{
	Epoll					epoll;
	std::map<int, Client>	clientList;

	initServer(serverList, epoll);
	while (g_running == true)
		eventLoop(serverList, clientList, epoll, epoll.wait());
}
