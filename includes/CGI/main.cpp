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


#include "../config/Config.hpp"
#include "../socket/Epoll.hpp"
#include "../socket/Socket.hpp"
#include "../socket/EpollSocket.hpp"
#include "CGI.hpp"
#include "../http/response.hpp"
#include "../http/request.hpp"

static void	handleConnection(Epoll &epoll, EpollSocket &local, std::pair<std::string, std::string> &clientInfo)
{
	EpollSocket	client(local.acceptConnection(), EPOLLIN | EPOLLET | EPOLLRDHUP);

	client.setNonBlocking();
	clientInfo.first = client.getNameInfo(client.listener(), NI_NUMERICHOST);
	clientInfo.second = client.getNameInfo(client.listener());
	epoll.control(EPOLL_CTL_ADD, client);
}

static void	handleDeconnection(Epoll &epoll, EpollSocket &client)
{
	epoll.control(EPOLL_CTL_DEL, client);
	client.shutdownSocket();
	client.closeSocket();
}

static void	handleInput(EpollSocket &client, const Config &serverConfig, const std::pair<std::string, std::string> &clientInfo)
{
	std::pair<std::string, int>	data;
	
	data = client.recvData();
	std::cout << data.first << std::endl;
	//httpRequest/httpResponse
	httpRequest 	request;
	httpResponse	response;
	request.treatRequest(data.first, serverConfig);
	if (request.getPath().find(".php") != std::string::npos){
		try{
			//serverConfig.servers[0] ->  will change depending on what server we work on
			//serverConfig.servers[0].locations[0] -> will change depending on what location we work on
			std::pair<ServerContext, LocationContext > serverLocation = 
			std::make_pair(serverConfig.servers[0], serverConfig.servers[0].locations[0]);

			CGI cgi(data, request.getHeaders(), serverLocation, clientInfo);
			cgi.CGIStartup();
		}
		catch(const std::exception &e){
			std::cout << e.what() << std::endl;
		}
	}
	response.buildResponse(request, serverConfig);
	response.sendResponse(client);
}

void	server(EpollSocket &local, const Config &serverConfig)
{
	int			nfds;
	Epoll		epoll;
	EpollSocket	socketEvent;
	std::pair<std::string, std::string> clientInfo;

	local.events(EPOLLIN | EPOLLRDHUP);
	epoll.control(EPOLL_CTL_ADD, local);
	while (true)
	{
		nfds = epoll.wait();
		for (int n = 0; n < nfds; ++n)
		{
			socketEvent = epoll.socketAt(n);
			if (socketEvent.listener() == local.listener())
				handleConnection(epoll, local, clientInfo);
			else if (socketEvent.events() & (EPOLLERR | EPOLLRDHUP | EPOLLHUP))
				handleDeconnection(epoll, socketEvent);
			else if (socketEvent.events() & EPOLLIN)
				handleInput(socketEvent, serverConfig, clientInfo);
		}
	}
}


int	main()
{
	Config	config;

	try {
		config.parse(std::string(WEBSERV_PATH) + "sites_availables/default.conf");
		std::cout << config.servers[0].locations[0].directives.at("cgi_extension")[0] << std::endl;
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
	try {
		EpollSocket	local;

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
