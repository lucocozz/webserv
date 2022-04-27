/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/24 19:32:27 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	handleInput(Client &client, Epoll &epoll)
{
	std::pair<std::string, int>					data;
	Server										*serverLink;
	const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	
	data = client.socket.recvData();
	serverLink = client.fetchServerlink(data.first);
	if (client.request.treatRequest(data.first, *serverLink) == true){
		client.response.buildResponse(&client.request, *serverLink, clientInfo);

		// executer ces lignes si le server doit envoyer une reponse au client
		client.socket.setEvents(client.socket.getEvents() | EPOLLOUT);
		epoll.control(EPOLL_CTL_MOD, client.socket);
	}
}
