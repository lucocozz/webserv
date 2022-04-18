/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configData.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:56 by user42            #+#    #+#             */
/*   Updated: 2022/04/18 16:51:29 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "configData.hpp"

#include "locationRelated.hpp"

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

size_t													getConfigMaxBodySize(Server const &server){
	try{
		if (atoi((server.context.directives.at("client_max_body_size")[0]).c_str()) < 100)
			return (atoi((server.context.directives.at("client_max_body_size")[0]).c_str()) * 1000);
		else
			return (1 * 1000);
	}
	catch (std::exception const &e){
		return (1 * 1000);
	}
}

std::pair<std::map<std::string, std::string>, bool>		getConfigErrorPage(Server const &server){
	std::pair<std::map<std::string, std::string>, bool> ret;
	try{
		std::vector<std::string> rawErrorPages = server.context.directives.at("error_page");
		std::string status;
		std::string path;
		std::map<std::string, std::string> map;
		for (size_t i = 0; i < rawErrorPages.size(); i++){
			if (i % 2 == 0)
				status = rawErrorPages.at(i);
			else{
				if (status.find_first_not_of("0123456789x") == std::string::npos){
					path = rawErrorPages.at(i);
					map.insert(std::make_pair(status, path));
				}
				status.clear();
				path.clear();
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
		try {std::vector<std::string> vec = (*it).directives.at("limit_except");}
		catch (std::exception const &e){needDel++;}
		if (needDel == 2){
			locations.erase(it);
			if (locations.empty() == true)
				break;
			it = locations.begin();
		}
	}
	return (locations);
}