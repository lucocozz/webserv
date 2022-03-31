/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:59:01 by user42            #+#    #+#             */
/*   Updated: 2022/04/01 00:44:50 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <dirent.h>
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

//Date
std::string					getActualTime(){
	time_t rawTime;
	struct tm *timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	std::vector<std::string> vect = split(asctime(timeInfo), " ");
	std::string ret;
	ret.append(vect.at(0) + ", ");
	std::string daydate;
	if (atoi(vect.at(2).c_str()) < 10)
		daydate = "0" + vect.at(2);
	else
		daydate = vect.at(2);
	ret.append(daydate + " ");
	ret.append(vect.at(1) + " ");
	std::string year = vect.at(4);
	year.erase(year.end() - 1);
	ret.append(year + " ");
	ret.append(vect.at(3) + " ");
	ret.append("GMT");

	return (ret);
}

//Last-Modified
std::string					getFileModification(std::string path){\
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string timeInfo = ctime(&sb.st_mtime);
	std::vector<std::string> vect = split(timeInfo, " ");
	std::string ret;
	try {
		ret.append(vect.at(0) + ", ");
		std::string daydate;
		if (atoi(vect.at(2).c_str()) < 10)
			daydate = "0" + vect.at(2);
		else
			daydate = vect.at(2);
		ret.append(daydate + " ");
		ret.append(vect.at(1) + " ");
		std::string year = vect.at(4);
		year.erase(year.end() - 1);
		ret.append(year + " ");
		ret.append(vect.at(3) + " GMT");
	}
	catch (std::exception const &e){
		ret.clear();
	}

	return (ret);
}

std::string					getFileSize(std::string path){\
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string ret(itos(sb.st_size));

	return (ret);
}

//ETag
std::string					makeETag(std::string &path){
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string ret;

	ret.append("W/\"");
	
	int mTime = sb.st_mtime;
	std::stringstream mTimeStream;
	mTimeStream << std::hex << mTime;
	ret.append(mTimeStream.str());
	
	ret.append("-");
	
	int sSize = sb.st_size;
	std::stringstream sSizeStream;
	sSizeStream << std::hex << sSize;
	ret.append(sSizeStream.str());
	
	ret.append("\"");

	return (ret);
}

std::string buildUrlTo(std::string host, std::string path, std::string fileName){
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

std::string	listFilesUrls(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host){
	std::string ret;
	while ((fileRead = readdir(rep)) != NULL){
		if (strlen(fileRead->d_name) != 0 && fileRead->d_name[0] != '.'){
			ret.append("<a href=\"");
			ret.append(buildUrlTo(host, path, fileRead->d_name));
			ret.append("\">");
			ret.append(fileRead->d_name);
			if (fileRead->d_type == DT_DIR)
				ret.append("/");
			ret.append("</a>");
			for (size_t i = strlen(fileRead->d_name); i < ((fileRead->d_type == DT_DIR) ? 54 : 55); i++)
				ret.append(" ");
			ret.append(getFileModification(buildPathTo(rootPath, path, fileRead->d_name)));
			for (size_t i = getFileModification(buildPathTo(rootPath, path, fileRead->d_name)).size(); i < 65; i++)
				ret.append(" ");
			if (fileRead->d_type == DT_DIR)
				ret.append("-");
			else
				ret.append(getFileSize(buildPathTo(rootPath, path, fileRead->d_name)));
			ret.append("\r\n");
		}
	}
	return (ret);
}

std::string	buildListingBlock(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host){
	std::string ret;
	ret.append("<pre>\r\n");
	if (path == rootPath)
		ret.append("<a href=\"" + buildUrlTo(host, path, "") + "\"><p>../</p></a>\r\n");
	else{
		ret.append("<a href=\"");
		ret.append(buildUrlTo(host, path, ".."));
		ret.append("\">../</a>\r\n");
	}
	ret.append(listFilesUrls(fileRead, rep, rootPath, path, host));
	ret.append("</pre>\r\n");
	return (ret);
}

#endif