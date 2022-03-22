/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleSignal.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 22:01:58 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/22 22:14:43 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include "server.hpp"
#include "system.hpp"

int	g_signal = 0;

void	handleSigint(int sig)
{
	g_signal = sig;
	g_running = false;
	std::cout << std::endl;
}
