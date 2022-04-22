/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileRelated.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:28 by user42            #+#    #+#             */
/*   Updated: 2022/04/21 23:55:33 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FILERELATED_HPP
#define FILERELATED_HPP

#include <string>
#include <vector>
#include <map>

std::string					formatDate(std::vector<std::string> vect);
std::string					buildDate();
std::string					formatLastModified(std::string path);
std::string					buildFileSize(std::string path);
std::string					formatETag(std::string &path);

std::map<std::string, std::string>		initExtensionTypes();

#endif
