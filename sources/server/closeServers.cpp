/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   closeServers.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/02 22:32:17 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/09 21:07:36 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void	closeServers(std::vector<Server> &serverList)
{
	EpollSocket			socket;
	std::vector<int>	closedSockets;

	for (size_t i = 0; i < serverList.size(); ++i)
	{
		socket = serverList[i].socket;
		if (std::find(closedSockets.begin(), closedSockets.end(), socket.listener()) == closedSockets.end())
		{
			closedSockets.push_back(socket.listener());
			socket.closeSocket();
		}
	}
}
