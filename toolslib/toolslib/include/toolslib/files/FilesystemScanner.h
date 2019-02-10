#ifndef _FILESYSTEM_SCANNER_H
#define _FILESYSTEM_SCANNER_H

#include <vector>

#include "toolslib/files/Filename.h"

namespace toolslib
{

namespace files
{

class TOOLSLIB_API FilenameScanner
{
public:
	typedef enum
	{
		SS_OK,					// Process the found entry
		SS_SKIP,				// Skip this entry and process the next one
		SS_ABORT				// Full stop scanning.

	} ScanState;

public:
	FilenameScanner(Filename const &oRoot, bool bIncludeSubdirectories = true);
	virtual ~FilenameScanner(void) {};

	bool includeSubdirectories(void) const { return mIncludeSubdirectories; }
	void includeSubdirectories(bool bIncludeSubdirectories) { mIncludeSubdirectories = bIncludeSubdirectories; }

	virtual std::vector<std::string> scan(void);
	virtual ScanState scan(Filename const &oRoot) = 0;

protected:
	std::vector<std::string> &Files(void) { return mFiles; };
	std::string const &Pattern(void) { return mPattern; };

private:
	Filename mRoot;
	std::vector<std::string> mFiles;
	std::string mPattern;
	bool mIncludeSubdirectories;
};

class TOOLSLIB_API FilesystemScanner
: public virtual FilenameScanner
{
public:
	FilesystemScanner(Filename const &oRoot, bool bIncludeSubdirectories = true);

	bool includeSubdirectories(void) const { return mIncludeSubdirectories; }
	void includeSubdirectories(bool bIncludeSubdirectories) { mIncludeSubdirectories = bIncludeSubdirectories; }

	FilenameScanner::ScanState scan(Filename const &oRoot) override;

private:
	typedef FilenameScanner super;

private:
	Filename mRoot;
	bool mIncludeSubdirectories;
};

}

}

#endif // _FILESYSTEM_SCANNER_H
