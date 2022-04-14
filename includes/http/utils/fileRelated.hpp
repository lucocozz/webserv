#ifndef FILERELATED_HPP
#define FILERELATED_HPP

#include "stringRelated.hpp"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

std::string					formatDate(std::vector<std::string> vect);
std::string					buildDate();
std::string					formatLastModified(std::string path);
std::string					buildFileSize(std::string path);
std::string					formatETag(std::string &path);

#endif