/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleOutput.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 21:19:14 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/26 20:13:31 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	handleOutput(Client &client, Epoll &epoll)
{
	if (client.response.getResponse().size() > 0)
	{
		client.response.sendResponse(client.socket);
		if (client.response.getResponse().size() == 0)
		{
			client.response.clear();
			client.socket.setEvents(client.socket.getEvents() & ~EPOLLOUT);
			epoll.control(EPOLL_CTL_MOD, client.socket);
		}
	}
}
