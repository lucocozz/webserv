/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleDeconnection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:59:57 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/30 19:29:33 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Server.hpp"
#include "EpollSocket.hpp"

void	handleDeconnection(Server &server, EpollSocket &socketEvent, Epoll &epoll)
{
	epoll.control(EPOLL_CTL_DEL, socketEvent);
	socketEvent.shutdownSocket();
	socketEvent.closeSocket();
	std::remove(server.clients.begin(), server.clients.end(), socketEvent);
}
