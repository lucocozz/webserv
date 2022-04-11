/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 14:37:30 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/11 19:28:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "EpollSocket.hpp"
#include "Server.hpp"

class Client
{
private:
	EpollSocket				_socket;
	std::vector<Server*>	_serverLinks;

public:
	Client(void) {}

	Client(EpollSocket &socket, std::vector<Server*> serverLinks):
		_socket(socket), _serverLinks(serverLinks) {}

	~Client(void) {}

	Client(const Client &rhs)
	{
		*this = rhs;
	}

	Client	&operator=(const Client &rhs)
	{
		if (this != &rhs)
		{
			this->_socket = rhs._socket;
			this->_serverLinks = rhs._serverLinks;
		}
		return (*this);
	}

	EpollSocket				getSocket() const{
		return (this->_socket);
	}
	std::vector<Server*>	getServerLinks() const{
		return (this->_serverLinks);
	}
};
