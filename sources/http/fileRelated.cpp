/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileRelated.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:00 by user42            #+#    #+#             */
/*   Updated: 2022/04/15 00:58:10 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fileRelated.hpp"

std::string					formatDate(std::vector<std::string> vect){
	std::string ret;
	for (std::vector<std::string>::iterator it = vect.begin(); it != vect.end(); it++){
		if ((*it).empty() == true){
			vect.erase(it);
			it = vect.begin();
		}
	}
	std::string daydate;
	if (atoi(vect.at(2).c_str()) < 10)
		daydate = "0" + vect.at(2);
	else
		daydate = vect.at(2);
	ret.append(daydate + ", ");
	ret.append(vect.at(1) + " ");
	std::string year = vect.at(4);
	year.erase(year.end() - 1);
	ret.append(year + " ");
	ret.append(vect.at(3) + " GMT");
	return (ret);
}

std::string					buildDate(){
	time_t rawTime;
	struct tm *timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);
	std::vector<std::string> vect = split(asctime(timeInfo), " ");
	std::string ret;

	try {
		ret.append(formatDate(vect));
	}
	catch (std::exception const &e){
		ret.clear();
	}

	return (ret);
}

std::string					formatLastModified(std::string path){\
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string timeInfo = ctime(&sb.st_mtime);
	std::vector<std::string> vect = split(timeInfo, " ");
	std::string ret;
	try {
		ret.append(formatDate(vect));
	}
	catch (std::exception const &e){
		ret.clear();
	}

	return (ret);
}

std::string					buildFileSize(std::string path){\
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string ret(itos(sb.st_size));

	return (ret);
}

std::string					formatETag(std::string &path){
	struct stat sb;
	lstat(path.c_str(), &sb);
	std::string ret;

	ret.append("W/\"");
	
	int mTime = sb.st_mtime;
	std::stringstream mTimeStream;
	mTimeStream << std::hex << mTime;
	ret.append(mTimeStream.str());
	
	ret.append("\"");

	return (ret);
}