/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleInput.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:55:31 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/22 21:56:16 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "EpollSocket.hpp"

void	handleInput(EpollSocket &client)
{
	std::pair<std::string, int>	data;
	
	data = client.recvData();
	std::cout << data.first << std::endl;
	//CGI_startup temporaire
	// CGI_startup(client, data);
	client.sendData("HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/plain\r\nContent-Length: 27\r\n\r\nResponse sent by 42 webserv");
}
