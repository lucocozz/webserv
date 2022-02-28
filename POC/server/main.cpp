/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/23 23:06:55 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"

void	server(EpollSocket &local)
{
	int			nfds;
	Epoll		epoll;
	EpollSocket	socketEvent;
	EpollSocket	client;

	local.events(EPOLLIN);
	epoll.control(EPOLL_CTL_ADD, local);
	while (true)
	{
		nfds = epoll.wait();
		for (int n = 0; n < nfds; ++n)
		{
			if (epoll.eventAt(n) & EPOLLIN)
			{
				socketEvent = epoll.socketAt(n);
				if (socketEvent.listener() == local.listener())
				{
					client = EpollSocket(local.acceptConnection(), EPOLLIN);
					client.setNonBlocking();
					epoll.control(EPOLL_CTL_ADD, client);
				}
				else if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
				{
					epoll.control(EPOLL_CTL_DEL, socketEvent);
					socketEvent.closeSocket();
				}
				else
				{
					std::pair<std::string, int>	data;
					data = socketEvent.recvData();
					std::cout << data.first << std::endl;
					if (data.second == 0)
					{
						epoll.control(EPOLL_CTL_DEL, socketEvent);
						socketEvent.closeSocket();
					}
					else
						socketEvent.sendData("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 27\r\n\r\nResponse sent by 42 webserv");
				}
			}
		}
	}
}

int	main()
{
	try {
		EpollSocket	local;

		local.createSocket(AF_INET, SOCK_STREAM, "8080");
		local.bindSocket();
		local.listenSocket();
		server(local);
		local.closeSocket();
	}
	catch (const std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
	return (0);
}
