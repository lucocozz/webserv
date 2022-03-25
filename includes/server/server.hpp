/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:19:24 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/25 10:19:51 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include "Config.hpp"
#include "Epoll.hpp"
#include "EpollSocket.hpp"

extern bool	g_running;

void						handleInput(EpollSocket &client, Config &config);
void						handleConnection(Epoll &epoll, EpollSocket &client);
void						handleDeconnection(Epoll &epoll, EpollSocket &client);
std::vector<EpollSocket>	listenServers(Config &config);
void						server(std::vector<EpollSocket> &localServers, Config &config);
