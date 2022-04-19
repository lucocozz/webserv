/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCore.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:19:24 by lucocozz          #+#    #+#             */
/*   Updated: 2022/04/19 21:33:40 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "Config.hpp"
#include "Epoll.hpp"
#include "EpollSocket.hpp"
#include "Server.hpp"
#include "Client.hpp"

extern bool	g_running;

std::vector<Server>	createServers(Config &config);
void				closeServers(std::vector<Server> &serverList);
void				handleConnection(std::vector<Server> &serverList,
						std::map<int, Client> &clientList, EpollSocket &socketEvent, Epoll &epoll);
void				handleDeconnection(std::map<int, Client> &clientList, EpollSocket &socketEvent, Epoll &epoll);
void				handleInput(Client &client, Epoll &epoll);
void				handleOutput(Client &client, Epoll &epoll);
void				serverCore(std::vector<Server> &serverList);
void				eventLoop(std::vector<Server> &serverList, std::map<int, Client> &clientList,
						Epoll &epoll, int events);
