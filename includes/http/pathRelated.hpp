/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pathRelated.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:39 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 00:58:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PATHRELATED_HPP
#define PATHRELATED_HPP

#include "stringRelated.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>

bool						isSameDirectory(std::string locationName, std::string path);
bool						isPathDirectory(std::string const &path);
bool						isPathValid(std::string const &path);
std::string 				buildPathTo(std::string rootPath, std::string path, std::string fileName);
int					removeDir(char const *path);

#endif