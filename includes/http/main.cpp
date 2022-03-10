/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/10 15:18:04 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../socket/Epoll.hpp"
#include "../socket/Socket.hpp"
#include "../socket/EpollSocket.hpp"

#include "../CGI/CGI.hpp"

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
					std::pair<std::string, int>	data = socketEvent.recvData();
					
					//httpRequest/httpResponse
					httpRequest 	request;
					httpResponse	response;

					if (data.second != 0){
						//CGI_startup temporaire
						CGI cgi(NULL, data);
						int cgiRet = 0;
						//httpRequest/httpResponse
						request.treatRequest(data.first);
						if (request.getPath().find(".php") != std::string::npos)
							cgiRet = cgi.CGIStartup(request.getHeaders());
						if (cgiRet > 0)
							std::cerr << "CGI error: " << errno <<std::endl;
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
