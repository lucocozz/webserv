/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 22:47:38 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/18 19:42:21 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>
#include <exception>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include "EpollSocket.hpp"

class Epoll
{
private:
	int					_epollFd;
	const int			_maxEvents;
	struct epoll_event	*_eventList;

public:
	Epoll(int maxEvents = 10): _maxEvents(maxEvents), _eventList(NULL)
	{
		this->_epollFd = epoll_create1(0);
		if (this->_epollFd == -1)
			throw (std::runtime_error(strerror(errno)));
		this->_eventList = new struct epoll_event[maxEvents];		
	}

	~Epoll()
	{
		close(this->_epollFd);
		delete[] this->_eventList;
	}

	void	control(int option, EpollSocket &target)
	{
		if (epoll_ctl(this->_epollFd, option, target.listener(), &target.infoEvent()) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	int	wait(int timeout = -1)
	{
		int	nfds;

		nfds = epoll_wait(this->_epollFd, this->_eventList, this->_maxEvents, timeout);
		if (nfds == -1)
			throw (std::runtime_error(strerror(errno)));
		return (nfds);
	}

	struct EpollSocket socketAt(int index)
	{
		if (index < 0 || index >= this->_maxEvents)
			throw (std::out_of_range("Epoll: socketAt() index out of range"));
		return (this->_eventList[index]);
	}

	uint32_t	eventAt(int index)
	{
		if (index < 0 || index >= this->_maxEvents)
			throw (std::out_of_range("Epoll: eventAt() index out of range"));
		return (this->_eventList[index].events);
	}
};
