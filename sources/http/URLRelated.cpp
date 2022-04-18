/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   URLRelated.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:45 by user42            #+#    #+#             */
/*   Updated: 2022/04/18 16:48:20 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "URLRelated.hpp"

#include "pathRelated.hpp"
#include "fileRelated.hpp"
#include <cstring>

std::string 				buildUrlTo(std::string host, std::string path, std::string fileName){
	std::string ret;

	ret.append("http://");
	ret.append(host);
	if (path != "/")
		ret.append(path);
	if (fileName.empty() == false){
		if (*(ret.end() - 1) != '/')
			ret.append("/");
		ret.append(fileName);
	}

	return (ret);
}

std::string					buildListFilesUrls(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host){
	std::string ret;
	while ((fileRead = readdir(rep)) != NULL){
		if (strlen(fileRead->d_name) != 0 && fileRead->d_name[0] != '.'){
			ret.append("<a href=\"");
			ret.append(buildUrlTo(host, path, fileRead->d_name));
			ret.append("\">");
			ret.append(fileRead->d_name);
			if (fileRead->d_type == DT_DIR)
				ret.append("/");
			ret.append("</a>");
			for (size_t i = strlen(fileRead->d_name); i < ((fileRead->d_type == DT_DIR) ? 54 : 55); i++)
				ret.append(" ");
			ret.append(formatLastModified(buildPathTo(rootPath, path, fileRead->d_name)));
			for (size_t i = formatLastModified(buildPathTo(rootPath, path, fileRead->d_name)).size(); i < 65; i++)
				ret.append(" ");
			if (fileRead->d_type == DT_DIR)
				ret.append("-");
			else
				ret.append(buildFileSize(buildPathTo(rootPath, path, fileRead->d_name)));
			ret.append("\r\n");
		}
	}
	return (ret);
}

std::string					buildListingBlock(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host){
	std::string ret;
	ret.append("<pre>\r\n");
	if (path == rootPath)
		ret.append("<a href=\"" + buildUrlTo(host, path, "") + "\"><p>../</p></a>\r\n");
	else{
		ret.append("<a href=\"");
		ret.append(buildUrlTo(host, path, ".."));
		ret.append("\">../</a>\r\n");
	}
	ret.append(buildListFilesUrls(fileRead, rep, rootPath, path, host));
	ret.append("</pre>\r\n");
	return (ret);
}