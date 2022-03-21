/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/18 15:04:31 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../config/Config.hpp"
#include "../socket/Epoll.hpp"
#include "../socket/Socket.hpp"
#include "../socket/EpollSocket.hpp"
#include "../CGI/CGI.hpp"
#include "response.hpp"
#include "request.hpp"

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

static void	handleInput(EpollSocket &client, Config const &config)
{
	std::pair<std::string, int>	data;
	httpRequest 	request;
	httpResponse	response;
	
	data = client.recvData();
	request.treatRequest(data.first, config);
	
	//DEBUG
	{
		std::cout << std::endl << "Parsed request :" << std::endl;
		std::cout << "Method = " << request.getMethod() << std::endl;
		std::cout << "Path = " << request.getPath() << std::endl;
		std::cout << "Protocol = " << request.getProtocol() << std::endl;
		std::cout << "Headers = " << std::endl;
		std::map<std::string, std::string> headers = request.getHeaders();
		for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
			std::cout << "    " << (*it).first << ": " << (*it).second << std::endl;
		std::cout << "Body = " << request.getBody() << std::endl;
		std::cout << "Status = " << request.getStatus() << std::endl << std::endl;
	}

	response.buildResponse(request, config);
	response.sendResponse(client);
}

void	server(EpollSocket &local, Config const &config)
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
				handleInput(socketEvent, config);
		}
	}
}

int	main()
{
	try {
		EpollSocket	local;
		Config		config;

		(void)config;
		config.parse(std::string(WEBSERV_PATH) + "sites_availables/default.conf");
		local.createSocket(AF_INET, SOCK_STREAM, "8080");
		local.setSockOpt(SOL_SOCKET, SO_REUSEADDR, 1);
		local.bindSocket();
		local.listenSocket();
		server(local, config);
		local.closeSocket();
	}
	catch (const std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
	return (0);
}
