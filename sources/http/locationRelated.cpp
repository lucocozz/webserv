/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationRelated.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:14 by user42            #+#    #+#             */
/*   Updated: 2022/04/21 17:04:17 by user42           ###   ########.fr       */
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

std::pair<bool,LocationContext>  getLocation(std::string path,const std::vector<LocationContext> &serverLocation){
	LocationContext 				init;
	std::pair<bool,LocationContext> locationPair = std::make_pair(true, init);
	
	for (size_t i = 0; i < serverLocation.size(); i++){
		if ((path.append("/").find(serverLocation[i].args[0]) != std::string::npos && 
			(serverLocation[i].directives.count("cgi_extension") == 1 && path.find(serverLocation[i].directives.find("cgi_extension")->second[0]) != std::string::npos)) ||
			((match(serverLocation[i].args[0].c_str(), path.c_str(), '*') == true) && serverLocation[i].directives.count("cgi_binary") == 1)){
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

bool						isMethodAllowed(std::vector<LocationContext> locations, std::string path, std::string method, std::vector<std::string> allowedRoot){
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
}

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

std::string		locationToRoot(std::string path, std::string rootLocation, std::string locationName){
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

	//DEBUG
	//std::cout << "DEBUG LOCATION:" << std::endl;
	//for (std::vector<LocationContext>::iterator it = locations.begin(); it != locations.end(); it++){
	//	std::cout << "location= " << (*it).args[0] << std::endl;
	//	for (std::map<std::string, std::vector<std::string> >::iterator itt = (*it).directives.begin(); itt != (*it).directives.end(); itt++){
	//		std::cout << "    " << (*itt).first << std::endl;
	//	}
	//}

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

std::string				retrieveUploadLocationName(std::vector<LocationContext> locations, std::string path){
	std::string ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "upload_location");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> uploadLocation = retrieveDirectiveArgs(isLocation.second, "upload_location");
	if (uploadLocation.empty() == false)
		ret = isLocation.second.args[0];
	return (ret);
}

std::string				retrieveRootLocationName(std::vector<LocationContext> locations, std::string path){
	std::string ret;

	std::pair<bool,LocationContext> isLocation = pathIsLocation(path, locations, "root");
	if (isLocation.first == false)
		return (ret);
	std::vector<std::string> root = retrieveDirectiveArgs(isLocation.second, "root");
	if (root.empty() == false)
		ret = isLocation.second.args[0];
	return (ret);
}