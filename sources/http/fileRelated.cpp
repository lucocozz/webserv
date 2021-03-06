/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileRelated.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: user42 <user42@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/04/15 00:58:00 by user42            #+#    #+#             */
/*   Updated: 2022/04/30 14:02:08 by user42           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "fileRelated.hpp"
#include "stringRelated.hpp"

#include <sstream>
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>

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

std::string							unchunkBody(std::string rawRequest){
	std::string unchunkedBody;
	std::string rawBody = rawRequest.substr(rawRequest.find("\r\n\r\n") + 4);
	size_t contentLength = 0;
	bool chunkReading = false;
	int chunkSize = 0;
	int chunkMaxSize = -1;
	std::string hexSize;
	for (std::string::iterator it = rawBody.begin(); it != rawBody.end(); it++){
		if (chunkReading == false && *it != '\r'){
			hexSize.push_back(*it);
		}
		else if (chunkReading == false && *it == '\r' && hexSize.empty() == false){
			it++;
			std::stringstream ss;
			ss << std::hex << hexSize;
			ss >> chunkMaxSize;
			chunkReading = true;
			if (chunkMaxSize == 0)
				break;
		}
		else if (chunkReading == true){
			unchunkedBody.push_back(*it);
			contentLength++;
			chunkSize++;
		}
		if (chunkSize == chunkMaxSize && chunkMaxSize != -1){
			chunkReading = false;
			hexSize.clear();
			chunkSize = 0;
			chunkMaxSize = -1;
		}
	}
	return (unchunkedBody);
}

std::map<std::string, std::string>	initExtensionTypes(){
	std::map<std::string, std::string> ret;

	//Text
	ret.insert(std::make_pair("css", "text/css"));
	ret.insert(std::make_pair("csv", "text/csv"));
	ret.insert(std::make_pair("html", "text/html"));
	ret.insert(std::make_pair("htm", "text/html"));
	ret.insert(std::make_pair("ics", "text/calendar"));
	ret.insert(std::make_pair("js", "text/javascript"));
	ret.insert(std::make_pair("txt", "text/plain"));
	//Image
	ret.insert(std::make_pair("bmp", "image/bmp"));
	ret.insert(std::make_pair("gif", "image/gif"));
	ret.insert(std::make_pair("ico", "image/vnd.microsoft.icon"));
	ret.insert(std::make_pair("jpeg", "image/jpeg"));
	ret.insert(std::make_pair("jpg", "image/jpeg"));
	ret.insert(std::make_pair("png", "image/png"));
	ret.insert(std::make_pair("svg", "image/svg+xml"));
	ret.insert(std::make_pair("tif", "image/tiff"));
	ret.insert(std::make_pair("webp", "image/webp"));
	//Audio
	ret.insert(std::make_pair("aac", "audio/aac"));
	ret.insert(std::make_pair("mid", "audio/midi"));
	ret.insert(std::make_pair("midi", "audio/midi"));
	ret.insert(std::make_pair("audio", "audio/mpeg"));
	ret.insert(std::make_pair("ogg", "audio/ogg"));
	ret.insert(std::make_pair("opus", "audio/opus"));
	ret.insert(std::make_pair("wav", "audio/wav"));
	ret.insert(std::make_pair("weba", "audio/webn"));
	//Video
	ret.insert(std::make_pair("avi", "video/x-msvideo"));
	ret.insert(std::make_pair("mpeg", "video/mpeg"));
	ret.insert(std::make_pair("ogv", "video/ogg"));
	ret.insert(std::make_pair("ts", "video/mp2t"));
	ret.insert(std::make_pair("webm", "video/webm"));
	ret.insert(std::make_pair("3gp", "video/3gpp"));
	ret.insert(std::make_pair("3g2", "video/3gpp2"));
	//Application
	ret.insert(std::make_pair("abw", "application/x-abiword"));
	ret.insert(std::make_pair("arc", "application/x-freearc"));
	ret.insert(std::make_pair("azw", "application/vnd.amazon.ebook"));
	ret.insert(std::make_pair("bin", "application/octet-stream"));
	ret.insert(std::make_pair("bz", "application/x-bzip"));
	ret.insert(std::make_pair("bz2", "application/x-bzip2"));
	ret.insert(std::make_pair("csh", "application/x-csh"));
	ret.insert(std::make_pair("doc", "application/msword"));
	ret.insert(std::make_pair("docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"));
	ret.insert(std::make_pair("eot", "application/vnd.ms-fontobject"));
	ret.insert(std::make_pair("epub", "application/epub+zip"));
	ret.insert(std::make_pair("gz", "application/gzip"));
	ret.insert(std::make_pair("jar", "application/java-archive"));
	ret.insert(std::make_pair("json", "application/json"));
	ret.insert(std::make_pair("jsonld", "application/ld+json"));
	ret.insert(std::make_pair("mpkg", "application/vnd.apple.installer+xml"));
	ret.insert(std::make_pair("odp", "application/vnd.oasis.opendocument.presentation"));
	ret.insert(std::make_pair("ods", "application/vnd.oasis.opendocument.spreadsheet"));
	ret.insert(std::make_pair("odt", "application/vnd.oasis.opendocument.text"));
	ret.insert(std::make_pair("ogx", "application/ogg"));
	ret.insert(std::make_pair("pdf", "application/pdf"));
	ret.insert(std::make_pair("php", "application/x-httpd-php"));
	ret.insert(std::make_pair("ppt", "application/vnd.ms-powerpoint"));
	ret.insert(std::make_pair("pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"));
	ret.insert(std::make_pair("rar", "application/vnd.rar"));
	ret.insert(std::make_pair("rtf", "application/rtf"));
	ret.insert(std::make_pair("sh", "application/x-sh"));
	ret.insert(std::make_pair("swf", "application/x-shockwave-flash"));
	ret.insert(std::make_pair("tar", "application/x-tar"));
	ret.insert(std::make_pair("vsd", "application/vnd.visio"));
	ret.insert(std::make_pair("xhtml", "application/xhtml+xml"));
	ret.insert(std::make_pair("xls", "application/vnd.ms-excel"));
	ret.insert(std::make_pair("xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"));
	ret.insert(std::make_pair("xml", "application/xml"));
	ret.insert(std::make_pair("xul", "application/vnd.mozilla.xul+xml"));
	ret.insert(std::make_pair("zip", "application/zip"));
	ret.insert(std::make_pair("7z", "application/x-7z-compressed"));

	return (ret);
}
