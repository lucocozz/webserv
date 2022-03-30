/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handleSignal.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lucocozz <lucocozz@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 22:01:58 by lucocozz          #+#    #+#             */
/*   Updated: 2022/03/30 17:52:59 by lucocozz         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <csignal>
#include <iostream>
#include "serverCore.hpp"
#include "system.hpp"

int	g_signal = 0;

void	handleSigint(int sig)
{
	g_signal = sig;
	g_running = false;
	std::cout << std::endl;
}
