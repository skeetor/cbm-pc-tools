#ifndef _ZIP_SCANNER_H
#define _ZIP_SCANNER_H

#include <vector>
#include <unzip.h>

#include "toolslib/files/FilesystemScanner.h"

namespace toolslib
{

namespace files
{

class TOOLSLIB_API ZIPScanner
: public virtual FilenameScanner
{
public:
	ZIPScanner(Filename const &oRoot, bool bIncludeSubdirectories = true);
	virtual ~ZIPScanner(void);

protected:
	ScanState scan(Filename const &oRoot) override;

	/**
	 * Read all files from the archive into the list.
	 */
	ScanState collectFiles(Filename const &oRoot, std::vector<std::string> &oFilelist);

private:
	typedef FilenameScanner super;

private:
	unzFile mFileHandle;
};

}

}

#endif // _ZIP_SCANNER_H
