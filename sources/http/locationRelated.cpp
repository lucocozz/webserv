/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationRelated.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:14 by user42            #+#    #+#             */
/*   Updated: 2022/04/30 00:22:59 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//pas sur
#include "stringRelated.hpp"
#include "locationRelated.hpp"
#include "pathRelated.hpp"

std::pair<bool,LocationContext>  pathIsLocation(std::string path,const std::vector<LocationContext> &serverLocation, std::string directiveName){
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

static bool isCgi(std::string path){
	size_t					dot = 0;
	std::string				extension("");
	std::string::iterator 	itb = path.begin();
	std::string::iterator 	ite;

	if ((dot = path.find(".")) == std::string::npos)
		return(false);
	itb += dot;
	ite = itb;
	while (*ite != '/' && *ite != '?' && ite != path.end())
		ite++;
	extension.append(itb, ite);
	if (extension != ".php" && extension != ".py")
		return (false);
	return(true);
}

std::pair<bool,LocationContext>  cgiChecker(std::string path,const std::vector<LocationContext> &serverLocation){
	LocationContext 				init;
	std::pair<bool,LocationContext> locationPair = std::make_pair(false, init);

	if (isCgi(path) == false)
		return (locationPair);
	for (size_t i = 0; i < serverLocation.size(); i++){
		if ((path.find(serverLocation[i].args[0]) != std::string::npos && 
			(serverLocation[i].directives.count("cgi_extension") == 1 && path.find(serverLocation[i].directives.find("cgi_extension")->second[0]) != std::string::npos))){
			locationPair.first = true;
			locationPair.second = serverLocation[i];
			return(locationPair);
		}
	}
	for (size_t i = 0; i < serverLocation.size(); i++){
		if ((path.find(serverLocation[i].args[0]) != std::string::npos && 
			(serverLocation[i].directives.count("cgi_extension") == 1 && path.find(serverLocation[i].directives.find("cgi_extension")->second[0]) != std::string::npos)) ||
			((match(path.c_str(), serverLocation[i].args[0].c_str(), '*') == true) && serverLocation[i].directives.count("cgi_binary") == 1)){
			locationPair.first = true;
			locationPair.second = serverLocation[i];
			return(locationPair);
		}
	}
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

/*bool						isMethodAllowed(std::vector<LocationContext> locations, std::string path, std::string method, std::vector<std::string> allowedRoot){
	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "allowed_method");
	if (isLocation.first == false && allowedRoot.empty() == false){
		for (size_t i = 0; i < allowedRoot.size(); i++){
			if (method == allowedRoot.at(i))
				return (true);
		}
		return (false);
	}
	else if (isLocation.first == false)
		return (true);

	std::vector<std::string> allowedMethods = retrieveDirectiveArgs(isLocation.second, "allowed_method");
	for (size_t i = 0; i < allowedMethods.size(); i++){
		if (method == allowedMethods.at(i))
			return (true);
	}
	return (false);
}*/

std::string					checkIndex(std::string rootPath, std::vector<std::string> index){
	std::string ret;

	for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); it++){
		if (isPathValid(buildPathTo(rootPath, *it, "")) == true)
			return (*it);
	}
	return ("default_index.html");
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

bool											retrieveLocationAutoIndex(std::vector<LocationContext> locations, std::string path){
	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "autoindex");
	if (isLocation.first == false)
		return (false);
	std::vector<std::string> autoindex = retrieveDirectiveArgs(isLocation.second, "autoindex");
	if (autoindex.empty() == false){
		if (autoindex.at(0) == "on")
			return (true);
		else
			return (false);
	}
	else
		return (false);
}

std::pair<std::string, std::string>				retrieveLocationRoot(std::vector<LocationContext> locations, std::string rootPath, std::string path){
	std::pair<std::string, std::string> ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "root");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> root = retrieveDirectiveArgs(isLocation.second, "root");
	if (root.empty() == false)
		ret.first = root.at(0);
	else
		ret.first = rootPath;
	ret.second = isLocation.second.args[0];
	
	return (ret);
}

std::string										locationToRoot(std::string path, std::string rootLocation, std::string locationName){
	size_t i = 0;
	if (*(locationName.end() - 1) != '/')
		locationName.append("/");
	if (*(rootLocation.end() - 1) != '/')
		rootLocation.append("/");
	std::string::iterator itPath = path.begin();
	std::string::iterator itLocation = locationName.begin();
	while (itPath != path.end() && itLocation != locationName.end()){
		if (*itPath != *itLocation && *itLocation != '/'){
			break;
		}
		itPath++;
		itLocation++;
		i++;
	}
	std::string ret;
	ret = path.replace(0, i, rootLocation);
	return (ret);
}

std::pair<bool, std::string>					retrieveLocationUpload(std::vector<LocationContext> locations, std::string path){
	std::pair<bool, std::string> ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "upload_location");
	if (isLocation.first == false){
		ret.first = false;
		return (ret);
	}
	std::vector<std::string> uploadLocation = retrieveDirectiveArgs(isLocation.second, "upload_location");
	if (uploadLocation.empty() == false){
		ret.first = true;
		ret.second = uploadLocation.at(0);
	}
	else
		ret.first = false;
	return (ret);
}

std::string										retrieveUploadLocationName(std::vector<LocationContext> locations, std::string path){
	std::string ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "upload_location");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> uploadLocation = retrieveDirectiveArgs(isLocation.second, "upload_location");
	if (uploadLocation.empty() == false)
		ret = isLocation.second.args[0];
	return (ret);
}

std::string										retrieveRootLocationName(std::vector<LocationContext> locations, std::string path){
	std::string ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "root");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> root = retrieveDirectiveArgs(isLocation.second, "root");
	if (root.empty() == false)
		ret = isLocation.second.args[0];
	return (ret);
}

bool											doesStatusExist(int status){
	if (status >= 100 && status <= 511){
		if (status / 100 == 1 && status % 100 <= 3)
			return (true);
		else if (status / 100 == 2 && (status % 100 <= 8 || status % 100 == 26))
			return (true);
		else if (status / 100 == 3 && (status % 100 <= 8 || status % 100 == 10))
			return (true);
		else if (status / 100 == 4 && (status % 100 <= 18 || (status % 100 >= 21 && status % 100 <= 26) || (status % 100 >= 28 && status % 100 <= 31) || (status % 100 >= 49 && status % 100 <= 51) || status % 100 == 56))
			return (true);
		else if (status / 100 == 5 && status % 100 <= 11)
			return (true);
	}
	return (false);
}

std::pair<int, std::string>						retrieveLocationRedirection(std::vector<LocationContext> locations, std::string path){
	std::pair<int, std::string> ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "return");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> redirection = retrieveDirectiveArgs(isLocation.second, "return");
	if (redirection.empty() == false){
		ret.first = atoi(redirection.at(0).c_str());
		//need better check if status code is valid
		if (doesStatusExist(ret.first) == true){
			if (redirection.size() > 2 && !((ret.first >= 301 && ret.first <= 303) || ret.first == 307)){
				for (size_t i = 1; i < redirection.size(); i++){
					ret.second.append(redirection.at(i));
					if (i != redirection.size() - 1)
						ret.second.append(" ");
				}
			}
			else if (redirection.size() == 2){
				ret.second = redirection.at(1);
			}
		}
		else
			ret.first = 0;
	}
	return (ret);
}
