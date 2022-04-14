#ifndef URLRELATED_HPP
#define URLRELATED_HPP

#include "pathRelated.hpp"
#include "fileRelated.hpp"

std::string 				buildUrlTo(std::string host, std::string path, std::string fileName);
std::string					buildListFilesUrls(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host);
std::string					buildListingBlock(struct dirent *fileRead, DIR *rep, std::string rootPath, std::string path, std::string host);

#endif