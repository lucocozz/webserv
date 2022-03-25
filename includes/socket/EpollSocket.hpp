/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollSocket.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 23:19:48 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/23 19:14:14 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EPOLLSOCKET_HPP
# define EPOLLSOCKET_HPP

#include <string>
#include <sys/epoll.h>
#include "Socket.hpp"

class EpollSocket: public Socket
{
private:
	struct epoll_event	_infoEvent;

public:
	explicit EpollSocket(int fd = 0, uint32_t events = 0): Socket(fd)
	{
		bzero(&this->_infoEvent, sizeof(this->_infoEvent));
		this->_infoEvent.data.fd = fd;
		this->_infoEvent.events = events;
	}

	explicit EpollSocket(struct epoll_event infoEvent): Socket(infoEvent.data.fd)
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

	void	createSocket(int family, int socktype, std::string port, int flags = 0)
	{
		Socket::createSocket(family, socktype, port, flags);
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

#endif
