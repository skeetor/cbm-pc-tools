#ifndef _WILDCARDS_H
#define _WILDCARDS_H

#include <stdio.h>  // For va_start, etc.
#include <stdarg.h>  // For va_start, etc.

#include "toolslib/toolslib_api.h"
#include "toolslib/toolslib_def.h"

#ifndef NEGATE_CHAR
#define NEGATE_CHAR	'^'			// std cset negation char
#endif

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <functional>

namespace toolslib
{

namespace strings
{

/**
 * Returns true if pString matches the wildcard pattern.
 *
 * The pattern is a UNIX style pattern, which means that "*.*" does NOT match the string "a"
 * if a filename should be checked, and requires a "a.x" to satisfy or "*" instead.
 *
 * It includes also ranges like i.e. "[B-M]" for an uppercase character or negation "[!B-M]" 
 * for lowercase as "[b-m]" or [A-Z0-9] for uppercase and numbers, etc..
 *
 * It can be chosen to ignore the case (bCaseSensitive == true || false)
 * and pattern characters can be escaped using the '\' character if bAllowEscape == true.
 * i.E. "A\*B*" matches "A*Bx" but not "AxBx" because the first wildcard is ignored in this case.
 *
 * Note that '\' must be '\\' in an hardcoded string,
 * i.e. char *p = "A\\*B";
 */
bool matchesWildcard(const char *pPattern, const char *pString, const char *pEnd, bool bCaseSensitive, bool bAllowEscape);
/**
 * Checks if a string contains a wildcard pattern. Note that this function may return true even if there is no valid
 * wildcard if the string contains something that looks like a wildcard.
 */
bool hasWildcard(const char *pPattern, const char *pEnd = NULL);

}

}

#endif // _WILDCARDS_H
