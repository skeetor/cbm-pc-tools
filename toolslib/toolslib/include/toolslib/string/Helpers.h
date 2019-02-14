#ifndef _STRING_HELPERS_H
#define _STRING_HELPERS_H

#include <cctype>
#include <string>
#include <vector>

#include "toolslib/toolslib_api.h"
#include "toolslib/toolslib_def.h"

namespace toolslib
{

namespace strings
{
/**
 * Looks if one of the strings from the list is contained in the inputstring. If found, the index
 * in the array is returned and nPosition is set to the index in the inputstring where it starts.
 * If no substring is found, then nPosition is unchanged.
 */
size_t TOOLSLIB_API findSubstring(std::string const &oString, std::vector<std::string> const &oStringList, size_t &nPosition);

/**
 * Same as findSubstring only it ensures that the first occurence is found if the string contains multiple entries.
 * i.E. if a string like ./myfile.zip/file.gz is passed in and ".gz" is before ".zip" in the list, findSubstring()
 * would return the index to ".gz" while findFirstSubstring will return ".zip".
 */
size_t TOOLSLIB_API  findFirstSubstring(std::string const &oString, std::vector<std::string> const &oStringList, size_t &nPosition);

/**
 * If bExclude == true it removes all entries from the array that starts with the specified filter.
 * If bExclude = false, all entries will be removed where entry does NOT start with the filter.
 *
 * Note that if oFilter is a reference to a string in the array itself, it may get removed and as consequence
 * the oFilter is changed without notice (even though it is a const!!!), so make sure that oFilter is
 * is not pointing in the array itself and create a seperate copy in this case.
 */
size_t TOOLSLIB_API filter(std::vector<std::string> &oArray, std::string const &oFilter, bool bCaseSensitive = true, bool bExclude = true);

// static locals for inlining
namespace
{
	char toUpper(char c)
	{
		return (char)::toupper(c);
	}

	char toLower(char c)
	{
		return (char)::tolower(c);
	}

	void toUpperStr(std::string& value)
	{
		for (char & c : value) c = (char)::toupper(c);
	}

	void toLowerStr(std::string& value)
	{
		for (char & c : value) c = (char)::tolower(c);
	}
}

}

}

#endif // _STRING_HELPERS_H
