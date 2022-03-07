/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/07 16:56:41 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Socket.hpp"
#include "EpollSocket.hpp"

#include "../CGI/CGI.hpp"

//httpRequest
#include "../http/httpResponse.hpp"

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
					//httpRequest/httpResponse
					httpRequest 	request;
					httpResponse	response;

					if (data.second != 0){
						//CGI_startup temporaire
						CGI::CGI_startup(NULL, data);
						//httpRequest/httpResponse
						request.treatRequest(data.first);
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
