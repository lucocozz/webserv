/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleDeconnection.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:59:57 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/26 15:34:04 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "Server.hpp"
#include "EpollSocket.hpp"
#include "Client.hpp"

void	handleDeconnection(std::map<int, Client> &clientList, EpollSocket &socketEvent, Epoll &epoll)
{
	epoll.control(EPOLL_CTL_DEL, socketEvent);
	socketEvent.closeSocket();
	clientList.erase(socketEvent.listener());
}
