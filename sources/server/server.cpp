/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:14:35 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/10 17:38:48 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"

// #include "../CGI/ClassCGI.hpp"

static void	handleConnection(Epoll &epoll, EpollSocket &local)
{
	EpollSocket	client(local.acceptConnection(), EPOLLIN | EPOLLET | EPOLLRDHUP);

	client.setNonBlocking();
	epoll.control(EPOLL_CTL_ADD, client);
}

static void	handleDeconnection(Epoll &epoll, EpollSocket &client)
{
	epoll.control(EPOLL_CTL_DEL, client);
	client.shutdownSocket();
	client.closeSocket();
}

static void	handleInput(EpollSocket &client)
{
	std::pair<std::string, int>	data;
	
	data = client.recvData();
	std::cout << data.first << std::endl;
	//CGI_startup temporaire
	// CGI_startup(client, data);
	client.sendData("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 27\r\n\r\nResponse sent by 42 webserv");
}

void	server(EpollSocket &local)
{
	int			nfds;
	Epoll		epoll;
	EpollSocket	socketEvent;

	local.events(EPOLLIN | EPOLLRDHUP);
	epoll.control(EPOLL_CTL_ADD, local);
	while (true)
	{
		nfds = epoll.wait();
		for (int n = 0; n < nfds; ++n)
		{
			socketEvent = epoll.socketAt(n);
			if (socketEvent.listener() == local.listener())
				handleConnection(epoll, local);
			else if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
				handleDeconnection(epoll, socketEvent);
			else if (socketEvent.events() & EPOLLIN)
				handleInput(socketEvent);
		}
	}
}
