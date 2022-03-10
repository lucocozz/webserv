/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   headersFunctions.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/10 15:59:01 by user42            #+#    #+#             */
/*   Updated: 2022/03/10 16:07:57 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADERSFUNCTIONS_HPP
#define HEADERSFUNCTIONS_HPP

#include "utils.hpp"

//Date
std::string					getActualTime(){
	time_t rawTime;
	struct tm *timeInfo;
	time(&rawTime);
	timeInfo = localtime(&rawTime);

	std::vector<std::string> vect = split(asctime(timeInfo), " ");
	std::string ret;
	ret.append(vect.at(0) + ", ");
	std::string daydate;
	if (atoi(vect.at(2).c_str()) < 10)
		daydate = "0" + vect.at(2);
	else
		daydate = vect.at(2);
	ret.append(daydate + " ");
	ret.append(vect.at(1) + " ");
	std::string year = vect.at(4);
	year.erase(year.end() - 1);
	ret.append(year + " ");
	ret.append(vect.at(3) + " ");
	ret.append("GMT");

	return (ret);
}

//Last-Modified
std::string					getFileModification(char const *path){
	struct stat sb;
	lstat(path, &sb);
	std::string timeInfo = ctime(&sb.st_mtime);
	std::vector<std::string> vect = split(timeInfo, " ");
	std::string ret;

	ret.append(vect.at(0) + ", ");
	std::string daydate;
	if (atoi(vect.at(3).c_str()) < 10)
		daydate = "0" + vect.at(3);
	else
		daydate = vect.at(3);
	ret.append(daydate + " ");
	ret.append(vect.at(1) + " ");
	std::string year = vect.at(5);
	year.erase(year.end() - 1);
	ret.append(year + " ");
	ret.append(vect.at(4) + " GMT");

	return (ret);
}

//ETag
std::string					makeETag(char const *path){
	struct stat sb;
	lstat(path, &sb);
	std::string ret;

	ret.append("\"");
	int mTime = sb.st_mtime;
	std::stringstream mTimeStream;
	mTimeStream << std::hex << mTime;
	ret.append(mTimeStream.str());
	ret.append("-");
	int sSize = sb.st_size;
	std::stringstream sSizeStream;
	sSizeStream << std::hex << sSize;
	ret.append(sSizeStream.str());
	ret.append("\"");

	return (ret);
}

#endif