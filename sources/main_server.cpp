/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/08 17:30:22 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./Epoll.hpp"
#include "./Socket.hpp"
#include "./EpollSocket.hpp"

// #include "../CGI/ClassCGI.hpp"

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
			{
				EpollSocket	client(local.acceptConnection(), EPOLLIN | EPOLLET | EPOLLRDHUP);

				client.setNonBlocking();
				epoll.control(EPOLL_CTL_ADD, client);
			}
			else if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
			{
				epoll.control(EPOLL_CTL_DEL, socketEvent);
				socketEvent.closeSocket();
			}
			else if (socketEvent.events() & EPOLLIN)
			{
				std::string	data = socketEvent.recvData();
				std::cout << data << std::endl;
				//CGI_startup temporaire
				// CGI_startup(client, data);
				socketEvent.sendData("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 27\r\n\r\nResponse sent by 42 webserv");
			}
		}
	}
}

int	main()
{
	try {
		EpollSocket	local;

		local.createSocket(AF_INET, SOCK_STREAM, "8080");
		local.setSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
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
