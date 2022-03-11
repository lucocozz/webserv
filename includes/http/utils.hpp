/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:59:01 by user42            #+#    #+#             */
/*   Updated: 2022/03/11 15:15:49 by user42           ###   ########.fr       */
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

bool						doesFileExist(std::string const &file){
	//need to retrieve the server path
	std::string serverPath = "/home/user42";
	serverPath.append(file);
	if (access(serverPath.c_str(), F_OK) == 0){
		std::cout << "DEBUG : path = " << serverPath << " | ret = " << access(serverPath.c_str(), F_OK) << std::endl;
		return true;
	}
	else{
		std::cout << "DEBUG : path = " << serverPath << " | ret = " << access(serverPath.c_str(), F_OK) << std::endl;
		return false;
	}
}

std::string					itos(int nb){
	std::stringstream str;
	str << nb;
	std::string ret;
	str >> ret;
	return (ret);
}

#endif