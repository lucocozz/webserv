/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/01/23 22:47:36 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/17 16:22:50 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "Epoll.hpp"
#include "Socket.hpp"

// void	server(EpollSocket &local)
// {
// 	int		nfds;
// 	Epoll	epoll(MAX_EVENTS);

// 	local.event(EPOLLIN);
// 	epoll.control(EPOLL_CTL_ADD, local.listener(), local.event());
// 	while (true)
// 	{
// 		nfds = epoll.wait();
// 		for (int n = 0; n < nfds; ++n)
// 		{
// 			if (eventList[n].data.fd == local.listener())
// 			{
				
// 			}
// 		}
// 	}
// }

int	main()
{
	try {
		Socket	client;
		Socket	local(AF_INET, SOCK_STREAM, AI_PASSIVE, "8081");
		std::string	response("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nResponse sent by 42 webserv");

		local.bindSocket();
		local.listenSocket();
		client = local.acceptConnection();
		std::cout << local.recvData(client) << std::endl;
		local.sendData(client, response);
		local.closeSocket();
		client.closeSocket();
	}
	catch (const std::exception &e) {
		std::cerr << "error: " << e.what() << std::endl;
	}
	return (0);
}
