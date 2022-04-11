/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:58:10 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/09 21:07:36 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Server.hpp"
#include "EpollSocket.hpp"
#include "Client.hpp"

static std::vector<Server*>	getLinkedServer(std::vector<Server> &serverList, EpollSocket &socketEvent)
{
	std::vector<Server*>	servers;

	for (size_t i = 0; i < serverList.size(); ++i)
	{
		if (serverList[i].socket == socketEvent)
			servers.push_back(&serverList[i]);
	}
	return (servers);
}

void	handleConnection(std::vector<Server> &serverList,
	std::map<int, Client> &clientList, EpollSocket &socketEvent, Epoll &epoll)
{
	EpollSocket	client(socketEvent.acceptConnection(), EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP);

	client.setNonBlocking();
	epoll.control(EPOLL_CTL_ADD, client);
	clientList.insert(std::make_pair(client.listener(), Client(client, getLinkedServer(serverList, client))));
}
