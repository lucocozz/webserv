/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleOutput.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 21:19:14 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/24 18:36:35 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	handleOutput(Client &client, Epoll &epoll)
{
	client.response.sendResponse(client.socket);
	// exectuer ces lignes si le client a envoyer toutes ses données
	client.socket.setEvents(client.socket.getEvents() & ~EPOLLOUT);
	epoll.control(EPOLL_CTL_MOD, client.socket);
}
