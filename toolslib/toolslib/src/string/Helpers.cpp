#include <windows.h>

#include <toolslib/string/Helpers.h>

namespace toolslib
{

namespace strings
{

using namespace std;

size_t TOOLSLIB_API findSubstring(string const &oString, vector<string> const &oStringList, size_t &nPosition)
{
	for(vector<string>::const_iterator it = oStringList.begin(); it != oStringList.end(); *it++)
	{
		size_t pos = oString.find(*it);
		if(pos != string::npos)
		{
			nPosition = pos;
			return it - oStringList.begin();
		}
	}

	return string::npos;
}

size_t TOOLSLIB_API findFirstSubstring(string const &oString, vector<string> const &oStringList, size_t &nPosition)
{
	size_t index = string::npos;

	for(vector<string>::const_iterator it = oStringList.begin(); it != oStringList.end(); *it++)
	{
		size_t pos = oString.find(*it);
		if(pos != string::npos)
		{
			nPosition = pos;
			size_t i = it - oStringList.begin();
			if(index == string::npos || index > i)
				index = i;
		}
	}

	return index;
}

}

}
