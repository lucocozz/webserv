/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stringRelated.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:41 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 00:58:50 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "stringRelated.hpp"

std::vector<std::string>	split(std::string str, std::string delimiter){
	size_t posStart = 0;
	size_t posEnd = 0;
	size_t delimLen = delimiter.length();
	std::string token;
	std::vector<std::string> ret;

	while ((posEnd = str.find(delimiter, posStart)) != std::string::npos){
		token = str.substr(posStart, posEnd - posStart);
		posStart = posEnd + delimLen;
		ret.push_back(token);
	}
	ret.push_back(str.substr(posStart));
	return (ret);
}

std::string					itos(int nb){
	std::stringstream str;
	str << nb;
	std::string ret;
	str >> ret;
	return (ret);
}

bool							match(char const *s1, char const *s2, char wildcard){
	if (*s1 != '\0' && *s2 == wildcard)
		return (match(s1 + 1, s2, 'x') || match(s1, s2 + 1, 'x'));
	if (*s1 == '\0' && *s2 == wildcard)
		return (match(s1, s2 + 1, 'x'));
	if (*s1 == *s2 && *s1 != '\0' && *s2 != '\0')
		return (match(s1 + 1, s2 + 1, 'x'));
	if (*s1 == *s2 && *s1 == '\0' && *s2 == '\0')
		return (true);
	return (false);
}