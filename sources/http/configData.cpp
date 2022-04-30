/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:56 by user42            #+#    #+#             */
/*   Updated: 2022/04/30 16:06:27 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configData.hpp"
#include "locationRelated.hpp"
#include "stringRelated.hpp"
#include "pathRelated.hpp"
#include "statusCode.hpp"

std::string												getConfigServerName(Server const &server){
	try{
		std::vector<std::string> server_name = server.context.directives.at("server_name");
		return (server_name[0]);
	}
	catch (std::exception const &e){
		return ("server");
	}
}

std::string												getConfigRootPath(Server const &server){
	try{
		std::vector<std::string> root_path = server.context.directives.at("root");
		return (root_path[0]);
	}
	catch (std::exception const &e){
		return ("/");
	}
}

std::string 											getConfigIndex(Server const &server, std::string _rootPath){
	try{
		std::vector<std::string> index = server.context.directives.at("index");
		return (checkIndex(_rootPath, index));
	}
	catch (std::exception const &e){
		return ("default_index.html");
	}
}

bool													getConfigAutoIndex(Server const &server){
	try{
		std::string autoindex = server.context.directives.at("autoindex")[0];
		if (autoindex == "on")
			return (true);
		else
			return (false);
	}
	catch (std::exception const &e){
		return (false);
	}
}

std::vector<std::string>								getConfigAllowedMethod(Server const &server){
	std::vector<std::string> allowedMethod;
	try{
		allowedMethod = server.context.directives.at("allowed_method");

		return (allowedMethod);
	}
	catch (std::exception const &e){
		return (allowedMethod);
	}
}

size_t													getConfigMaxBodySize(Server const &server){
	try{
		std::string rawBodySize = server.context.directives.at("client_max_body_size")[0];
		int size = atoi(rawBodySize.substr(0, rawBodySize.size() - 1).c_str());
		char unit = *(rawBodySize.end() - 1);
		size_t finalSize = 0;
		if (unit == 'K')
			finalSize = size * 1000;
		else if (unit == 'M')
			finalSize = size * 1000000;
		else if (unit == 'G')
			finalSize = size * 1000000000;
		else
			finalSize = 1 * 1000;
		return (finalSize);
	}
	catch (std::exception const &e){
		return (1 * 1000000);
	}
}

std::pair<std::map<std::string, std::string>, bool>		getConfigErrorPage(Server const &server, std::string rootPath){
	std::pair<std::map<std::string, std::string>, bool> ret;
	try{
		std::vector<std::string> rawErrorPages = server.context.directives.at("error_page");
		std::string status;
		std::string path;
		std::map<std::string, std::string> map;
		for (size_t i = 0; i < rawErrorPages.size(); i++){
			std::vector<std::string> vec = split(rawErrorPages.at(i), ":");
			if (vec.size() == 2){
				status = vec.at(0);
				path = vec.at(1);
				if (status.find_first_not_of("0123456789x") == std::string::npos && isPathValid(buildPathTo(rootPath, path, "")))
					map.insert(std::make_pair(status, path));
			}
		}
		ret.first.insert(map.begin(), map.end());
		ret.second = true;
	}
	catch (std::exception const &e){
		ret.second = false;
	}
	return (ret);
}

std::vector<LocationContext>							getConfigLocations(Server const &server){
	std::vector<LocationContext> locations = server.context.locations;
	for (std::vector<LocationContext>::iterator it = locations.begin(); it != locations.end(); it++){
		int needDel = 0;
		try {std::vector<std::string> vec = (*it).directives.at("index");}
		catch (std::exception const &e){needDel++;}
		try {std::vector<std::string> vec = (*it).directives.at("allowed_method");}
		catch (std::exception const &e){needDel++;}
		try {std::vector<std::string> vec = (*it).directives.at("autoindex");}
		catch (std::exception const &e){needDel++;}
		try {std::vector<std::string> vec = (*it).directives.at("root");}
		catch (std::exception const &e){needDel++;}
		try {std::vector<std::string> vec = (*it).directives.at("upload_location");}
		catch (std::exception const &e){needDel++;}
		try {std::vector<std::string> vec = (*it).directives.at("return");}
		catch (std::exception const &e){needDel++;}
		if (needDel == 6){
			locations.erase(it);
			if (locations.empty() == true)
				break;
			it = locations.begin();
		}
	}
	return (locations);
}
