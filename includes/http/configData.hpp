/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:23 by user42            #+#    #+#             */
/*   Updated: 2022/05/01 15:17:24 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGDATA_HPP
#define CONFIGDATA_HPP


#include "Server.hpp"
//#include "locationRelated.hpp"

#include <vector>
#include <utility>

std::string												getConfigServerName(Server const &server);
std::string												getConfigRootPath(Server const &server);
std::pair<std::string, bool> 							getConfigIndex(Server const &server, std::string _rootPath);
bool													getConfigAutoIndex(Server const &server);
std::vector<std::string>								getConfigAllowedMethod(Server const &server);
size_t													getConfigMaxBodySize(Server const &server);
std::pair<std::map<std::string, std::string>, bool>		getConfigErrorPage(Server const &server, std::string _rootPath);
std::vector<LocationContext>							getConfigLocations(Server const &server);

#endif
