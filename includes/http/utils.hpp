/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:59:01 by user42            #+#    #+#             */
/*   Updated: 2022/04/14 19:02:36 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

/*
	Includes :
*/

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

/*
	Strings :
*/

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

/*
	Path :
*/

bool						isSameDirectory(std::string locationName, std::string path){
	if (locationName.find(path) == 0){
		if (locationName == path)
			return (true);
		else if (locationName.size() == path.size() + 1 && locationName.at(locationName.size() - 1) == '/')
			return (true);
	}
	return (false);
}

bool						isPathDirectory(std::string const &path){
	struct stat s;
	stat(path.c_str(), &s);
	if (s.st_mode & S_IFDIR)
		return (true);
	return (false);
}

bool						isPathValid(std::string const &path){
	struct stat sb;
	if (lstat(path.c_str(), &sb) == 0)
		return (true);
	return (false);
}

std::string 				buildPathTo(std::string rootPath, std::string path, std::string fileName){
	std::string ret;

	//rootPath
	ret.append(rootPath);
	//path
	if (*(ret.end() - 1) == '/' && *(path.begin()) == '/'){
		path.erase(path.begin());
		ret.append(path);
	}
	else if (path != "/")
		ret.append(path);

	//fileName
	if (fileName.empty() == false){
		if (*(ret.end() - 1) != '/')
			ret.append("/");
		ret.append(fileName);
	}

	return (ret);
}

int					removeDir(char const *path){
	struct dirent *entry = NULL;
	DIR *dir = NULL;
	dir = opendir(path);
	if (dir == NULL)
		return (-1);
	while ((entry = readdir(dir))){
		DIR *sub_dir = NULL;
		FILE *file = NULL;
		char abs_path[257] = {0};
		if (*(entry->d_name) != '.'){
			sprintf(abs_path, "%s/%s", path, entry->d_name);
			if ((sub_dir = opendir(abs_path))){
				closedir(sub_dir);
				removeDir(abs_path);
			}
			else{
				if ((file = fopen(abs_path, "r"))){
					fclose(file);
					remove(abs_path);
				}
			}
		}
	}
	remove(path);
	return (1);
}

/*
	Dates :
*/

std::string					formatDate(std::vector<std::string> vect){
	std::string ret;
	for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); it++){
		if ((*it).empty() == true){
			vect.erase(it);
			it = vect.begin();
		}
	}
	std::string daydate;
	if (atoi(vect.at(2).c_str()) < 10)
		daydate = "0" + vect.at(2);
	else
		daydate = vect.at(2);
	ret.append(daydate + ", ");
	ret.append(vect.at(1) + " ");
	std::string year = vect.at(4);
	year.erase(year.end() - 1);
	ret.append(year + " ");
	ret.append(vect.at(3) + " GMT");
	return (ret);
}

std::string					buildDate(){
	time_t rawTime;
	struct tm *timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	std::vector<std::string> vect = split(asctime(timeInfo), " ");
	std::string ret;

	try {
		ret.append(formatDate(vect));
	}
	catch (std::exception const &e){
		ret.clear();
	}

	return (ret);
}

std::string					buildLastModified(std::string path){\
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string timeInfo = ctime(&sb.st_mtime);
	std::vector<std::string> vect = split(timeInfo, " ");
	std::string ret;
	try {
		ret.append(formatDate(vect));
	}
	catch (std::exception const &e){
		ret.clear();
	}

	return (ret);
}

/*
	Files Related :
*/

std::string					buildFileSize(std::string path){\
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string ret(itos(sb.st_size));

	return (ret);
}

std::string					buildETag(std::string &path){
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string ret;

	ret.append("W/\"");
	
	int mTime = sb.st_mtime;
	std::stringstream mTimeStream;
	mTimeStream << std::hex << mTime;
	ret.append(mTimeStream.str());
	
	ret.append("\"");

	return (ret);
}

std::string					checkIndex(std::string rootPath, std::vector<std::string> index){
	std::string ret;

	for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++){
		if (isPathValid(buildPathTo(rootPath, *it, "")) == true)
			return (*it);
	}
	return ("default_index.html");
}

/*
	URL related :
*/

std::string 				buildUrlTo(std::string host, std::string path, std::string fileName){
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

std::string					buildListFilesUrls(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host){
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
			ret.append(buildLastModified(buildPathTo(rootPath, path, fileRead->d_name)));
			for (size_t i = buildLastModified(buildPathTo(rootPath, path, fileRead->d_name)).size(); i < 65; i++)
				ret.append(" ");
			if (fileRead->d_type == DT_DIR)
				ret.append("-");
			else
				ret.append(buildFileSize(buildPathTo(rootPath, path, fileRead->d_name)));
			ret.append("\r\n");
		}
	}
	return (ret);
}

std::string					buildListingBlock(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host){
	std::string ret;
	ret.append("<pre>\r\n");
	if (path == rootPath)
		ret.append("<a href=\"" + buildUrlTo(host, path, "") + "\"><p>../</p></a>\r\n");
	else{
		ret.append("<a href=\"");
		ret.append(buildUrlTo(host, path, ".."));
		ret.append("\">../</a>\r\n");
	}
	ret.append(buildListFilesUrls(fileRead, rep, rootPath, path, host));
	ret.append("</pre>\r\n");
	return (ret);
}

//Locations Directives
static std::pair<bool,LocationContext>  pathIsLocation(std::string path,const std::vector<LocationContext> &serverLocation, std::string directiveName){
	LocationContext init;
	std::pair<bool,LocationContext> locationPair = std::make_pair(true, init);

	for (size_t i = 0; i < serverLocation.size(); i++){
		if ((path.append("/").find(serverLocation[i].args[0]) != std::string::npos) && 
			((serverLocation[i].directives.count(directiveName) == 1) || (serverLocation[i].directives.count(directiveName) == 1))){
			locationPair.second = serverLocation[i];
			return(locationPair);
		}
	}
	locationPair.first = false;
	return (locationPair);
}

static std::pair<bool,LocationContext>  getLocation(std::string path,const std::vector<LocationContext> &serverLocation){
	LocationContext init;
	std::pair<bool,LocationContext> locationPair = std::make_pair(true, init);
	
	for (size_t i = 0; i < serverLocation.size(); i++){
		if ((path.append("/").find(serverLocation[i].args[0]) != std::string::npos) && 
			(serverLocation[i].directives.count("cgi_binary") == 1) &&
			(serverLocation[i].directives.count("cgi_extension") == 1) &&
			(path.find(serverLocation[i].directives.find("cgi_extension")->second[0])) != std::string::npos){
			locationPair.second = serverLocation[i];
			return(locationPair);
		}
	}
	locationPair.first = false;
	return (locationPair);
}

std::vector<std::string>				retrieveDirectiveArgs(LocationContext const &location, std::string directiveName){
	(void)directiveName;
	std::vector<std::string> ret;
	try{
		ret = (*location.directives.find(directiveName)).second;
	}
	catch (std::exception &e){
	}
	return (ret);
}

bool						isMethodAllowed(std::vector<LocationContext> locations, std::string path, std::string method){
	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "limit_except");
	if (isLocation.first == false)
		return (true);

	std::vector<std::string> limits = retrieveDirectiveArgs(isLocation.second, "limit_except");
	for (size_t i = 0; i < limits.size(); i++){
		if (method == limits.at(i))
			return (false);
	}
	return (true);
}

std::pair<std::string, std::string>				retrieveLocationIndex(std::vector<LocationContext> locations, std::string rootPath, std::string path){
	std::pair<std::string, std::string> ret; 

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "index");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> index = retrieveDirectiveArgs(isLocation.second, "index");
	ret.first = checkIndex(rootPath, index);
	ret.second = isLocation.second.args[0];
	return (ret);
}

#endif
