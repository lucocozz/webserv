/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/19 21:27:05 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

void	handleInput(Client &client, Epoll &epoll)
{
	// std::pair<std::string, int>					data;
	// std::string 								hostName("");
	// Server										*serverLink;
	// const std::pair<std::string, std::string> 	clientInfo(client.socket.getNameInfo(NI_NUMERICHOST), client.socket.getNameInfo());
	// httpRequest 								request;
	// httpResponse								response;

	// data = client.socket.recvData();
	// std::cout << "client datas: |" << data.first << "|" << std::endl;
	// if (data.first.find("Host") != std::string::npos){
	// 	std::string::iterator 	itb;
	// 	std::string::iterator	ite;

	// 	itb = data.first.begin() + data.first.find("Host:") + 6;
	// 	ite = itb;
	// 	while (*ite != '\n' && *ite != ':')
	// 		ite++;
	// 	hostName.append(itb, ite);
	// }
	// serverLink = client.getServerLinks(hostName);
	// request.treatRequest(data.first, *serverLink);
	// response.buildResponse(request, *serverLink, clientInfo);
	// response.sendResponse(client.socket);


	// executer ces lignes si le client doit envoyer des données
	client.socket.setEvents(client.socket.getEvents() | EPOLLOUT);
	epoll.control(EPOLL_CTL_MOD, client.socket);
}
