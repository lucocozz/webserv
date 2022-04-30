/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/30 02:11:52 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	handleInput(Client &client, Epoll &epoll)
{
	std::pair<std::string, int>					data;
	const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	
	try
	{
		data = client.socket.recvData();
	}
	catch (std::exception &e)
	{
		client.socket.setEvents(client.socket.getEvents() | EPOLLERR);
		epoll.control(EPOLL_CTL_MOD, client.socket);
		std::cerr << "recvData error: " << e.what() << std::endl;
	}
	if (client.linkedServer == NULL)
		client.fetchServerlink(data.first);
	if (client.request.treatRequest(data.first, *client.linkedServer) == true){
		client.response.buildResponse(&client.request, *client.linkedServer, clientInfo);
		// executer ces lignes si le server doit envoyer une reponse au client
		client.socket.setEvents(client.socket.getEvents() | EPOLLOUT);
		epoll.control(EPOLL_CTL_MOD, client.socket);
	}
}
