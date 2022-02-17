/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 22:47:38 by lucocozz          #+#    #+#             */
/*   Updated: 2022/02/17 17:11:50 by lucocozz         ###   ########.fr       */
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

#define MAX_EVENTS 10

class Epoll
{
private:
	int					_epollFd;
	const int			_max_events;
	struct epoll_event	*_events;

public:
	Epoll(int max_events): _max_events(max_events), _events(NULL)
	{
		this->_epollFd = epoll_create1(0);
		if (this->_epollFd == -1)
			throw (std::runtime_error(strerror(errno)));
		this->_events = new struct epoll_event[max_events];		
	}

	~Epoll()
	{
		close(this->_epollFd);
		delete[] this->_events;
	}

	void	control(int _operator_, int fd, struct epoll_event &event)
	{
		if (epoll_ctl(this->_epollFd, _operator_, fd, &event) == -1)
			throw (std::runtime_error(strerror(errno)));
	}

	int	wait(int timeout = -1)
	{
		int	nfds;

		nfds = epoll_wait(this->_epollFd, this->_events, this->_max_events, timeout);
		if (nfds == -1)
			throw (std::runtime_error(strerror(errno)));
		return (nfds);
	}
};
