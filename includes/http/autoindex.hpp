/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autoindex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/22 16:26:05 by user42            #+#    #+#             */
/*   Updated: 2022/03/25 01:20:07 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

std::string		buildAutoIndex(std::string rootPath, std::vector<std::string> indexPages){
	(void)rootPath;
	std::string ret;

	ret.append("<html>\r\n");
	ret.append("<head>\r\n");
	ret.append("</head>\r\n");
	ret.append("<body>\r\n");
	ret.append("<h1>Index of /</h1>\r\n");
	ret.append("<hr>\r\n");
	ret.append("<ul>\r\n");

	ret.append("<li><a href=\"http://localhost:8080/\">../</a></li>\r\n");

	if (indexPages.size() != 0){
		for (size_t i = 0; i < indexPages.size(); i++){
			//Need to verify if the index must be joignable or if it list every index mentionned in config file
			std::string indexLink = "<li><a href=\"http://localhost:8080/" + indexPages.at(i) + "\">" + indexPages.at(i) + "</a></li>\r\n";
			ret.append(indexLink);
			indexLink.clear();
		}
	}

	ret.append("</ul>\r\n");
	ret.append("</body>\r\n");
	ret.append("</html>\r\n");

	return (ret);
}