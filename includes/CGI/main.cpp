/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_server.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/10 13:11:47 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../socket/Epoll.hpp"
#include "../socket/Socket.hpp"
#include "../socket/EpollSocket.hpp"
#include "CGI.hpp"
#include "../http/response.hpp"
#include "../http/request.hpp"

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
				std::pair<std::string, int>	data = socketEvent.recvData();
				std::cout << data.first << std::endl;

				//httpRequest/httpResponse
				httpRequest 	request;
				httpResponse	response;

				if (data.second != 0){
						request.treatRequest(data.first);

						if (request.getPath().find(".php") != std::string::npos){
							try{
								CGI cgi(NULL, data, request.getHeaders());
								cgi.CGIStartup();
							}
							catch(std::exception &e){
								std::cout << e.what() << std::endl;
							}
						}

						response.buildResponse(request);
					}
				std::cout << data.first << std::endl;
				if (data.second == 0)
				{
					epoll.control(EPOLL_CTL_DEL, socketEvent);
					socketEvent.closeSocket();
				}
				else
					response.sendResponse(socketEvent);
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
