/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleConnection.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 21:58:10 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/22 21:59:31 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Epoll.hpp"
#include "EpollSocket.hpp"

void	handleConnection(Epoll &epoll, EpollSocket &local)
{
	EpollSocket	client(local.acceptConnection(), EPOLLIN | EPOLLET | EPOLLRDHUP);

	client.setNonBlocking();
	epoll.control(EPOLL_CTL_ADD, client);
}
