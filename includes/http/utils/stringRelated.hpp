#ifndef STRINGRELATED_HPP
#define STRINGRELATED_HPP

#include <utility>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>

std::vector<std::string>	split(std::string str, std::string delimiter);
std::string					itos(int nb);
bool							match(char const *s1, char const *s2, char wildcard);

#endif