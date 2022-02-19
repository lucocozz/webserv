/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollSocket.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 23:19:48 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/19 19:55:12 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "Epoll.hpp"

class EpollSocket: public Socket
{
private:
	struct epoll_event	_infoEvent;

public:
	EpollSocket(int fd = 0, uint32_t events = 0): Socket(fd)
	{
		this->_infoEvent.data.fd = fd;
		this->_infoEvent.events = events;
	}

	EpollSocket(struct epoll_event infoEvent): Socket(infoEvent.data.fd)
	{
		this->_infoEvent = infoEvent;
	}
	
	EpollSocket(const EpollSocket &socket): Socket(socket)
	{
		this->_infoEvent = socket._infoEvent;
	}

	~EpollSocket() {}

	EpollSocket	&operator=(const EpollSocket &rhs)
	{
		if (this != &rhs)
		{
			Socket::operator=(rhs);
			this->_infoEvent = rhs._infoEvent;
		}
		return (*this);
	}

	void	createSocket(int socktype = SOCK_STREAM)
	{
		Socket::createSocket(socktype);
		this->_infoEvent.data.fd = this->_listenSocket;
	}

	uint32_t events(int events)
	{
		this->_infoEvent.events = events;
		return (this->_infoEvent.events);
	}

	uint32_t events(void)
	{
		return (this->_infoEvent.events);
	}

	struct epoll_event	&infoEvent(void)
	{
		return (this->_infoEvent);
	}
};
