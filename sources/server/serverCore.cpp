/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCore.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:14:35 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/27 18:36:33 by lucocozz         ###   ########.fr       */
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
			socket.setEvents(EPOLLIN);
			epoll.control(EPOLL_CTL_ADD, socket);
			controledSocket.push_back(socket.listener());
		}
	}
}

static void	closeClient(std::map<int, Client> &clientList)
{
	for (std::map<int, Client>::iterator it = clientList.begin(); it != clientList.end(); ++it)
			it->second.socket.closeSocket();
}

void	serverCore(std::vector<Server> &serverList)
{
	Epoll					epoll(20);
	std::map<int, Client>	clientList;

	initServer(serverList, epoll);
	try {
		while (g_running == true)
			eventLoop(serverList, clientList, epoll, epoll.wait());
	}
	catch (std::exception &e)
	{
		if (errno == EINTR)
			closeClient(clientList);
		else
			std::cerr << "Error: " << e.what() << std::endl;
	}
}
