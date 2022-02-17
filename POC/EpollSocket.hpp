/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollSocket.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 23:19:48 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/16 23:27:44 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Epoll.hpp"

class EpollSocket: public Socket
{
private:
	struct epoll_event	_event;

public:
	EpollSocket() {}

	EpollSocket(Socket &socket)
	{
		*this = socket;
	}

	EpollSocket(int family, int socktype, int flags, std::string port): Socket(family, socktype, flags, port)
	{
		this->_event.data.fd = this->_listenSocket;
	}

	~EpollSocket() {}

	EpollSocket	&operator=(const EpollSocket &rhs)
	{
		if (this != &rhs)
		{
			this->_listenSocket = rhs.listener();
			// this->_bindAdress = rhs.addrInfo();
			//! check comment bien copy bindAddress
			this->_event = rhs._event;
		}
		return (*this);
	}

	EpollSocket	&operator=(const Socket &rhs)
	{
		if (this != &rhs)
		{
			this->_listenSocket = rhs.listener();
			// this->_bindAdress = rhs.addrInfo();
			//! check comment bien copy bindAddress (maybe getaddrinfo or memcpy)
		}
		return (*this);
	}

	struct epoll_event &event(int events)
	{
		this->_event.events = events;
		return (this->_event);
	}

	struct epoll_event &event(void)
	{
		return (this->_event);
	}
};
