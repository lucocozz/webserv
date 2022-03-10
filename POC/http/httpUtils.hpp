#ifndef HTTPUTILS_HPP
#define HTTPUTILS_HPP

#include <algorithm>
#include <vector>
#include <utility>

#include <sys/stat.h>
#include <sstream>

#include <iostream>
#include <ctime>
#include <cstdlib>

std::vector<std::string>	split(std::string str, std::string delimiter){
	size_t posStart = 0;
	size_t posEnd = 0;
	size_t delimLen = delimiter.length();
	std::string token;
	std::vector<std::string> ret;

	while ((posEnd = str.find(delimiter, posStart)) != std::string::npos){
		token = str.substr(posStart, posEnd - posStart);
		posStart = posEnd + delimLen;
		ret.push_back(token);
	}
	ret.push_back(str.substr(posStart));
	return (ret);
}

bool						isPathExist(std::string const &path){
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0);
}

std::string					itos(int nb){
	std::stringstream str;
	str << nb;
	std::string ret;
	str >> ret;
	return (ret);
}

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

#include <sys/types.h>
#include <unistd.h>

std::string		getFileModification(char const *path){
	struct stat sb;
	lstat(path, &sb);
	std::string timeInfo = ctime(&sb.st_mtime);
	std::vector<std::string> vect = split(timeInfo, " ");

	std::string ret;
	std::cout << "bug\n";
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

#endif