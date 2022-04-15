/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URLRelated.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:57:51 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 00:58:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef URLRELATED_HPP
#define URLRELATED_HPP

#include "pathRelated.hpp"
#include "fileRelated.hpp"

std::string 				buildUrlTo(std::string host, std::string path, std::string fileName);
std::string					buildListFilesUrls(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host);
std::string					buildListingBlock(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host);

#endif