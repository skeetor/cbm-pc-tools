#include "pch.h"

#include <vector>

#include "utils.h"

using namespace std;

static const char NEGATE_CHAR = '!';

bool hasWildcard(const char *pPattern, const char *pEnd)
{
	if (pEnd == NULL)
		pEnd = pPattern + strlen(pPattern);

	while (pPattern < pEnd)
	{
		char c = *pPattern;
		if (c == '*' || c == '?' || c == '[')
			return true;

		pPattern++;
	}

	return false;
}

bool matchesWildcard(const char *pPattern, const char *pString, const char *pEnd, bool bCaseSensitive, bool bAllowEscape)
{
	bool negate;
	bool match;
	char c;

	while (*pPattern)
	{
		if (pString >= pEnd && *pPattern != '*')
			return false;

		c = *pPattern++;
		switch (c)
		{
		case '*':
		{
			while (*pPattern == '*')
				pPattern++;

			if (!*pPattern)
				return true;

			if (*pPattern != '?' && *pPattern != '[')
			{
				if (bAllowEscape == false || bAllowEscape == true && *pPattern != '\\')
				{
					while (pString < pEnd && *pPattern != *pString)
						pString++;
				}
			}

			while (pString < pEnd)
			{
				if (matchesWildcard(pPattern, pString, pEnd, bCaseSensitive, bAllowEscape))
					return true;

				pString++;
			}
			return false;
		}
		break;

		case '?':
		{
			if (pString >= pEnd)
				break;

			return false;
		}
		break;

		/*
		 * set specification is inclusive, that is [a-z] is a, z and
		 * everything in between. this means [z-a] may be interpreted
		 * as a set that contains z, a and nothing in between.
		 */
		case '[':
		{
			if (*pPattern != NEGATE_CHAR)
				negate = false;
			else
			{
				negate = true;
				pPattern++;
			}

			match = false;

			while (!match && *pPattern)
			{
				c = *pPattern++;
				if (!*pPattern)
					return false;

				if (*pPattern == '-')	/* c-c */
				{
					if (!*++pPattern)
						return false;

					if (*pPattern != ']')
					{
						char sc = *pString;
						if (!bCaseSensitive)
						{
							sc = (toupper(sc) & 0xff);
							c = (toupper(c) & 0xff);
						}

						if (sc == c || *pString == *pPattern || (*pString > c && *pString < *pPattern))
							match = true;
					}
					else		/* c-] */
					{
						if (*pString >= c)
							match = true;

						break;
					}
				}
				else			/* cc or c] */
				{
					if (c == *pString)
						match = true;

					if (*pPattern != ']')
					{
						if (*pPattern == *pString)
							match = true;
					}
					else
						break;
				}
			}

			if (negate == match)
				return false;

			/*
			 * if there is a match, skip past the cset and continue on
			 */
			while (*pPattern && *pPattern != ']')
				pPattern++;

			if (!*pPattern++)	/* oops! Missing bracket ... */
				return false;
		}
		break;

		case '\\':
		{
			// If the escape character is allowed, then we have to ignore it and
			// check the next character, otherwise the escape character is treated
			// like any other character.
			if (bAllowEscape)
			{
				if (*pPattern)
					c = *pPattern++;
			}
		}

		default:
		{
			char sc = *pString;

			if (!bCaseSensitive)
			{
				sc = (toupper(sc) & 0xff);
				c = (toupper(c) & 0xff);
			}

			if (c != sc)
				return false;
		}
		break;
		}
		pString++;
	}

	return (pString <= pEnd);
}

void split(const string &str, vector<string> &tokens, const string &delimiters, bool trimEmpty)
{
	string::size_type pos, lastPos = 0, length = str.length();

	while (lastPos < length + 1)
	{
		pos = str.find_first_of(delimiters, lastPos);
		if (pos == string::npos)
			pos = length;

		if (pos != lastPos || !trimEmpty)
			tokens.push_back(string(str.data() + lastPos, pos - lastPos));

		lastPos = pos + 1;
	}
}

