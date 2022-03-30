/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serverCore.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:19:24 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/30 19:33:00 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "Config.hpp"
#include "Epoll.hpp"
#include "EpollSocket.hpp"
#include "Server.hpp"

extern bool	g_running;

std::vector<Server>			createServers(Config &config);
void						handleConnection(Server &server, EpollSocket &socketEvent, Epoll &epoll);
void						handleDeconnection(Server &server, EpollSocket &socketEvent, Epoll &epoll);
void						handleInput(Server &server, EpollSocket &socketEvent);
void						serverCore(std::vector<Server> &localServers);
