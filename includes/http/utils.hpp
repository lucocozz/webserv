/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:59:01 by user42            #+#    #+#             */
/*   Updated: 2022/03/10 16:09:34 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <vector>
#include <utility>
#include <sys/stat.h>
#include <sstream>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

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

bool						ifPathExist(std::string const &path){
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

std::string					itos(int nb){
	std::stringstream str;
	str << nb;
	std::string ret;
	str >> ret;
	return (ret);
}

#endif