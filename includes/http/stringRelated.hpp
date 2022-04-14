/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringRelated.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:48 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 00:58:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRINGRELATED_HPP
#define STRINGRELATED_HPP

#include <utility>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

std::vector<std::string>	split(std::string str, std::string delimiter);
std::string					itos(int nb);
bool							match(char const *s1, char const *s2, char wildcard);

#endif