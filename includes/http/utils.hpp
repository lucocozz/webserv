/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:59:01 by user42            #+#    #+#             */
/*   Updated: 2022/03/31 18:05:58 by user42           ###   ########.fr       */
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
	if (access(serverPath.c_str(), F_OK) == 0)
		return true;
	else
		return false;
}

std::string					itos(int nb){
	std::stringstream str;
	str << nb;
	std::string ret;
	str >> ret;
	return (ret);
}

int							match(char *s1, char *s2){
	if (*s1 != '\0' && *s2 == '*')
		return (match(s1 + 1, s2) || match(s1, s2 + 1));
	if (*s1 == '\0' && *s2 == '*')
		return (match(s1, s2 + 1));
	if (*s1 == *s2 && *s1 != '\0' && *s2 != '\0')
		return (match(s1 + 1, s2 + 1));
	if (*s1 == *s2 && *s1 == '\0' && *s2 == '\0')
		return (1);
	return (0);
}

bool	isPathDirectory(std::string const &path){
	struct stat s;
	stat(path.c_str(), &s);
	if (s.st_mode & S_IFDIR)
		return (true);
	return (false);
}

std::string buildUrl(std::string host, std::string path, std::string fileName){
	std::string ret;

	ret.append("http://");
	ret.append(host);
	if (path != "/")
		ret.append(path);
	if (fileName.empty() == false){
		if (*(ret.end() - 1) != '/')
			ret.append("/");
		ret.append(fileName);
	}

	return (ret);
}

std::string buildPathTo(std::string rootPath, std::string path, std::string fileName){
	std::string ret;

	ret.append(rootPath);
	if (path != "/")
		ret.append(path);
	//Si il y a 2 / j'enleve celui du path
	else if (*(ret.end() - 1) == '/' && *(path.begin()) == '/')
		path.erase(path.begin());
	if (fileName.empty() == false){
		if (*(ret.end() - 1) != '/')
			ret.append("/");
		ret.append(fileName);
	}

	return (ret);
}

#endif