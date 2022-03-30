/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:58:10 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/31 00:43:36 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Server.hpp"
#include "EpollSocket.hpp"

void	handleConnection(Server &server, EpollSocket &socketEvent, Epoll &epoll)
{
	EpollSocket	client(socketEvent.acceptConnection(), EPOLLIN | EPOLLET | EPOLLRDHUP);

	client.setNonBlocking();
	server.clients.push_back(client);
	epoll.control(EPOLL_CTL_ADD, client);
}
