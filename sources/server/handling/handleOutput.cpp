/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleOutput.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/19 21:19:14 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/19 21:35:53 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	handleOutput(Client &client, Epoll &epoll)
{
	// exectuer ces lignes si une fois que le client a envoyer toutes ses données
	client.socket.setEvents(client.socket.getEvents() & ~EPOLLOUT);
	epoll.control(EPOLL_CTL_MOD, client.socket);
}
