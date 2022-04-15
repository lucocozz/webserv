/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 14:37:30 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/14 14:25:19 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "EpollSocket.hpp"
#include "Server.hpp"

class Client
{
private:
	std::vector<Server*>	_serverLinks;

public:
	EpollSocket	socket;

	Client(void) {}

	Client(EpollSocket &socket, std::vector<Server*> serverLinks):
		_serverLinks(serverLinks), socket(socket) {}

	~Client() {}

	Client(const Client &rhs)
	{
		*this = rhs;
	}

	Client	&operator=(const Client &rhs)
	{
		if (this != &rhs)
		{
			this->socket = rhs.socket;
			this->_serverLinks = rhs._serverLinks;
		}
		return (*this);
	}

	Server	*getServerLinks(std::string hostName) const
	{
		if (this->_serverLinks.size() == 1)
			return (this->_serverLinks.at(0));
		for (size_t i = 0; i < this->_serverLinks.size(); ++i)
			if (this->_serverLinks.at(i)->context.directives.find("server_name")->second[0] == hostName)
				return (this->_serverLinks.at(i));
		return (this->_serverLinks.at(0));
	}
};
