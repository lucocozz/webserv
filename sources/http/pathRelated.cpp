/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathRelated.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:32 by user42            #+#    #+#             */
/*   Updated: 2022/04/19 22:01:53 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pathRelated.hpp"

//#include "stringRelated.hpp"
#include <sys/stat.h>
#include <sys/types.h>
//#include <unistd.h>
#include <dirent.h>
#include <cstdio>

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
	if(*(ret.end() - 1) != '/')
		ret.append("/");
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
		if (fileName != "/")
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
