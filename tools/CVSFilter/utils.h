#pragma once

#ifndef _UTILS_H
#define _UTILS_H

#include <string>
#include <algorithm>
#include <cctype>

static inline std::string toUpper(std::string oStr)
{
	std::transform(oStr.begin(), oStr.end(), oStr.begin(), ::toupper);
	return oStr;
}

static inline bool isNumber(const std::string& s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it)) ++it;
	return !s.empty() && it == s.end();
}

void split(const std::string &str, std::vector<std::string> &tokens, const std::string &delimiters = " ", bool trimEmpty = false);
static inline std::vector<std::string> split(const std::string &str, const std::string &delimiters = " ", bool trimEmpty = false)
{
	std::vector<std::string> tokens;
	split(str, tokens, delimiters, trimEmpty);
	return tokens;
}

bool hasWildcard(const char *pPattern, const char *pEnd);
bool matchesWildcard(const char *pPattern, const char *pString, const char *pEnd, bool bCaseSensitive, bool bAllowEscape);

#endif _UTILS_H
