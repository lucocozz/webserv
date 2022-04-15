/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 14:37:30 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/15 01:55:02 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "EpollSocket.hpp"
#include "Server.hpp"
//#include "response.hpp"

class Client
{
private:
	std::vector<Server*>	_serverLinks;

public:
	EpollSocket	socket;
	//httpRequest 			request;
	//httpResponse			response;

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

	Server	*getServerLinks(std::string hostName) const {
		if (this->_serverLinks.size() == 1)
			return (this->_serverLinks.at(0));
		size_t i = 0;
		while (i < this->_serverLinks.size()){
			if (this->_serverLinks.at(i)->context.directives.find("server_name")->second[0] == hostName)
				return (this->_serverLinks.at(i));
			i++;
		}
		return (this->_serverLinks.at(0));
	}
};
