/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   locationRelated.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:14 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 00:58:50 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "locationRelated.hpp"

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
		if ((path.append("/").find(serverLocation[i].args[0]) != std::string::npos && path.find(serverLocation[i].directives.find("cgi_extension")->second[0]) != std::string::npos) ||
			(match(serverLocation[i].args[0].c_str(), path.c_str(), '*') == true) ){
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