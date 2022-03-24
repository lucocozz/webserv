/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:19:24 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/22 22:11:47 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "Config.hpp"
#include "Epoll.hpp"
#include "EpollSocket.hpp"

extern bool	g_running;

void	handleInput(EpollSocket &client, const Config &serverConfig);
void	handleConnection(Epoll &epoll, EpollSocket &client);
void	handleDeconnection(Epoll &epoll, EpollSocket &client);
void	server(std::vector<EpollSocket> &localServers, Config &config);
