#ifndef CONFIGDATA_HPP
#define CONFIGDATA_HPP

#include "Server.hpp"
#include "utils/locationRelated.hpp"

std::string												getConfigServerName(Server const &server);
std::string												getConfigRootPath(Server const &server);
std::string 											getConfigIndex(Server const &server, std::string _rootPath);
bool													getConfigAutoIndex(Server const &server);
size_t													getConfigMaxBodySize(Server const &server);
std::pair<std::map<std::string, std::string>, bool>		getConfigErrorPage(Server const &server);
std::vector<LocationContext>							getConfigLocations(Server const &server);

#endif