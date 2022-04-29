/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleDeconnection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:59:57 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/26 18:11:45 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Server.hpp"
#include "EpollSocket.hpp"
#include "Client.hpp"

void	handleDeconnection(std::map<int, Client> &clientList, EpollSocket &socketEvent, Epoll &epoll)
{
	epoll.control(EPOLL_CTL_DEL, socketEvent);
	if (socketEvent.getEvents() & EPOLLRDHUP){
		try{
			socketEvent.shutdownSocket(SHUT_WR);
		}
		catch (std::exception &e)
		{
			clientList[socketEvent.listener()].response.clear();
			std::cerr << "Error during shutdownSocket: " << e.what() << std::endl;
		}
	}
	socketEvent.closeSocket();
	clientList.erase(socketEvent.listener());
}
