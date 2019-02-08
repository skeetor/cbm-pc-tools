#ifndef _FILENAME_H
#define _FILENAME_H

#include <string>

#include "toolslib/toolslib_api.h"
#include "toolslib/toolslib_def.h"

namespace toolslib
{

namespace files
{

static const char DEFAULT_PATH_DELIMITER = '\\';

/**
 * Filename provides functions related to filenames. A basepath can be
 * set, in which case the it is prepended to the filename. If the basepath
 * is not set (NULL), the current directory is assumed. If a full path
 * or an UNC path is used, then the basepath is ignored.
 */
class TOOLSLIB_API Filename
{
public:
	Filename(std::string const &oFilename, std::string const &oBasePath = "");
	Filename(char const *oFilename = NULL, char const *oBasePath = NULL);
	Filename(const Filename &oSource);
	virtual ~Filename(void);

	/**
	 * Matches a unix style pattern against the string and returns true if the string matches.
	 * this can also be used for DOS style searching, because the famous '*.*' is just a 
	 * special case of '*'.
	 *
	 * Unix style patterns allow for patterns like '[A-Z]*[0-9]' or negation, etc. also the patterns can
	 * be escaped using the '\' character if bAllowEscape = true. If set to false the '\' character will be
	 * treated like any other character, which makes handling of DOS paths easier. If the escape character is
	 * used and a DOS path is matched, then the '\' should either be replaced by '/' or by '\\'.
	 *
	 * i.E. '\*a' means a string like '*a' and not 'xa'.
	 *
	 * Note that this function is static, so it can also be called without an object, as it
	 * might be usefull in other cases.
	 *
	 */
	static bool matchesWildcard(const char *pPattern, const char *pString, const char *pEnd = NULL, bool bCaseSensitive = false, bool bAllowEscape = false);

	/**
	 * Returns -1 if the pattern contains no wildcards (regular string) or the index to the character
	 * where the first pattern starts.
	 *
	 * If bAllowEscape == true, then the escape character '\' is allowed and also considered as a pattern
	 * character.
	 */
	static size_t findWildcard(const char *pPattern, bool bAllowEscape = false);

	bool hasWildcard(bool bAllowEscape = false) const
	{
		return Filename::findWildcard(getOpenpath().c_str(), bAllowEscape) != invalid;
	}

	void setBasePath(std::string const &oBasePath)
	{
		mBasePath = oBasePath;
		updatePath();
	}
	void setBasePath(const char *oBasePath);

	/**
	 * Return the base with a separator.
	 */
	inline const std::string &getBasePath(void) const
	{
		return mBasePath;
	}

	/**
	 * Return the base without a separator.
	 */
	std::string getBaseDir(void) const;

	/**
	 * Return the directory of the filename without a separator.
	 */
	std::string getFileDir(void) const;

	/**
	 * Normalizes the filename. If the filename is set including
	 * a directory part, then this will split it into the
	 * separate components. Note that, after this, a call to
	 * getFilename() may return a different string then before.
	 *
	 * If basename is set and the filename also contains an absolute
	 * path, including a drive/UNC part, then this will take precedence
	 * and the basename is discarded.
	 *
	 * Example:
	 *		"d:\tmp":"tst.txt" result = "d:\tmp":"tst.txt"		<- no change
	 *		"d:\tmp":"t\tst.txt" result = "d:\tmp\t":"tst.txt"	<- basename changed
	 *		"":"d:\tmp\t\tst.txt" result = "d:\tmp\t":"tst.txt"	<- basename changed
	 */
	void normalize(void);

	void setFilename(const char *oFilename);
	void setFilename(std::string const &oFilename)
	{
		mFilename = oFilename;
		updatePath();
	}

	/**
	 * Return the filename (including the extension).
	 */
	inline const std::string &getFilename(void) const
	{
		return mFilename;
	}

	/**
	 * Return the filename without directory and extension.
	 */
	std::string getFilenamePart(void) const;

	/**
	 * getAbsolutePath() returns the full pathname to the file
	 * including the basepath. Note that this might be different
	 * from getOpenpath() even though they refer to the same
	 * file.
	 *
	 * Example: getOpenpath() returns "..\file.txt" while
	 *		getAbsolutePath() returns "D:\tmp\file.txt"
	 *
	 * Not implemented yet.
	 */
	inline std::string const &getAbsolutePath(void) const
	{
		return mOpenPath;
	}

	/**
	 * getOpenpath() returns the path to the file, including the
	 * base directory. When opening a file, this should be used.
	 */
	inline std::string const &getOpenpath(void) const
	{
		return mOpenPath;
	}

	inline void setDelimiter(char nDelimiter = DEFAULT_PATH_DELIMITER)
	{
		mDelimiter = nDelimiter;
	}

	inline char getDelimiter(void) const
	{
		return mDelimiter;
	}

	Filename &operator=(const Filename &oSource);
	Filename &copy(const Filename &oSource);

	/**
	 * Returns the root drive path of the file. This is either the drive
	 * or the adress in case of an UNC path. Currently this works only if
	 * this is part of the filename. If bFindDrive == true and the filename
	 * doesn't contain a drive part, then the drive will be located (may
	 * not be sucessfull though). If it is false, then only the filename
	 * is checked.
	 *
	 * Example:
	 *		D:\temp\file.txt => 'D:\'
	 *		\\MyServer\pathtofile\file.zip => '\\MyServer\'
	 *		D:\ => 'D:\'
	 *		\\MyServer\ => '\\MyServer\'
	 *		D: => 'D:\'
	 *		\\MyServer => '\\MyServer\'
	 *		mypath\to\filename.txt => ''
	 */
	std::string getDrivepath(void) const;
	std::string findDrive(const std::string &oPath) const;

	/**
	 * Return the extension of the openpath. If case == false the extension
	 * is returned as is. If true, then it will be converted to uppercase if
	 * bUppercase == true and lowercase if bUppercase = false.
	 */
	std::string getExtension(bool bCase = false, bool bUppercase = false) const;

protected:
	void updatePath(void);

private:
	void init(void);

private:
	std::string mBasePath;
	std::string mFilename;
	std::string mOpenPath;
	char mDelimiter;
};

}

}

#endif // _FILENAME_H
