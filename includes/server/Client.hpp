/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/09 14:37:30 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/17 23:00:55 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "Server.hpp"
#include "response.hpp"

class Client
{
private:
	std::vector<Server*>	_serverLinks;

public:
	EpollSocket		socket;
	httpRequest 	request;
	httpResponse	response;

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

	const std::vector<Server*>	&getServerLinks() const {
		return (this->_serverLinks);
	}

	Server	*fetchServerlink(std::string &data) const
	{
		if (this->_serverLinks.size() == 1)
			return (this->_serverLinks.at(0));
		std::string hostName("");
		if (data.find("Host:") != std::string::npos){
			std::string::iterator 	itb;
			std::string::iterator	ite;

			itb = data.begin() + data.find("Host:") + 6;
			ite = itb;
			while (*ite != '\n' && *ite != ':')
				ite++;
			hostName.append(itb, ite);
		}
		for (size_t i = 0; i < this->_serverLinks.size(); ++i)
			if (this->_serverLinks.at(i)->context.directives.find("server_name")->second[0] == hostName)
				return (this->_serverLinks.at(i));
		return (this->_serverLinks.at(0));
	}
};
 