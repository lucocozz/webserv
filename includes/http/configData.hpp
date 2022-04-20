/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:23 by user42            #+#    #+#             */
/*   Updated: 2022/04/18 16:42:41 by user42           ###   ########.fr       */
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
std::string 											getConfigIndex(Server const &server, std::string _rootPath);
bool													getConfigAutoIndex(Server const &server);
size_t													getConfigMaxBodySize(Server const &server);
std::pair<std::map<std::string, std::string>, bool>		getConfigErrorPage(Server const &server);
std::vector<LocationContext>							getConfigLocations(Server const &server);

#endif
