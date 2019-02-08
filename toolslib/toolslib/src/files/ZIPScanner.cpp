#include <windows.h>

#include <iostream>
#include <algorithm>

#include "toolslib/files/ZIPScanner.h"
#include "toolslib/string/Helpers.h"

namespace toolslib
{

namespace files
{

using namespace std;
using namespace toolslib;
using namespace toolslib::strings;

ZIPScanner::ZIPScanner(Filename const &oRoot, bool bIncludeSubdirectories)
	: super(oRoot, bIncludeSubdirectories)
{
	mFileHandle = NULL;
}

ZIPScanner::~ZIPScanner(void)
{
}

FilenameScanner::ScanState ZIPScanner::scan(Filename const &oRoot)
{
	vector<string> files;

	// Read all the files from the archive.
	if(collectFiles(oRoot, files) != FilenameScanner::SS_OK)
		return FilenameScanner::SS_ABORT;

	Filename f = oRoot;

	// If the zip file itself was the target, then we assume to scan the whole zipfile.
	if(f.getFilename().length() == 0)
		f.setFilename("*");

	// In a ZIP file the files can be in arbitrary order, meaning that files in one directory
	// are not neccessarily grouped together. If a file is added later, it will be added at the end
	// of the list. So we need to make sure that all directories are grouped together to
	// make it easier to loop through them and skip directories on request.
	sort(files.begin(), files.end());

	// If the filename is like '<BaseDir>/directory/*.files' we can discard everything
	// that is not inside the 'directory/' path.
	Filename remove(f.getFilename());
	remove.normalize();
	if(remove.getBaseDir() != "")
	{
		filter(files, remove.getBaseDir(), false, false);	// Exclude all files not in the original path.
		if(files.size() > 0)								// Remove the original directory because we want to enter it for sure
			files.erase(files.begin());
	}

	string const &archive = oRoot.getBasePath();
	for(vector<string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		string const &file = *it;
		bool isDir = file[file.size()-1] == '\\';

		if(!isDir)
		{
			// Create the full filename with the archive and the path inside the archive
			if(Filename::matchesWildcard(Pattern().c_str(), file.c_str(), false, false))
				Files().push_back(archive+file);
		}
	}

	return FilenameScanner::SS_OK;
}

/**
 * Prepares the ZIP directory entries for scanning. A Zip files
 * has a flat hierarchy of directory entries, so we can not
 * traverse the tree recursively. In order to skip particular
 * directories, we have to filter them on request.
 */
FilenameScanner::ScanState ZIPScanner::collectFiles(Filename const &oRoot, vector<string> &oFilelist)
{
	if((mFileHandle = unzOpen64(oRoot.getBaseDir().c_str())) == NULL)
	{
		cerr << "Unable to open " << oRoot.getBaseDir() << endl;
		return FilenameScanner::SS_ABORT;
	}

	unz_global_info64 zip_info;
	if(unzGetGlobalInfo64(mFileHandle, &zip_info) != UNZ_OK)
		return FilenameScanner::SS_ABORT;

	if(!zip_info.number_entry)			// Empty zip file.
		return FilenameScanner::SS_OK;

	if(unzGoToFirstFile(mFileHandle) != UNZ_OK)		// corrupted zip file?
		return FilenameScanner::SS_ABORT;

	// reserve space for all our entries, so it doesn't need to reallocate on the fly.
	oFilelist.reserve((size_t)zip_info.number_entry);

	unz_file_info64 file_info;
	char filename[2001];

	// Now we collect all the files
	int rc = UNZ_OK;
	while(rc == UNZ_OK)
	{
		if(unzGetCurrentFileInfo64(mFileHandle, &file_info, filename, sizeof(filename), NULL, NULL, NULL, NULL) != UNZ_OK)
			return FilenameScanner::SS_ABORT;

		// The pattern will likely be in DOS style but the ZIP contains internally Unix style
		// directory seperators, so we have to convert it.
		char *p = filename;
		while(*p)
		{
			if(*p ==  '/')
				*p = '\\';

			p++;
		}

		// Ignore empty files or the root directory (not sure of this can even happen, but it can't hurt either)
		if(p-filename == 0)
			continue;

		if(p-filename == 1)
		{
			if(p[0] == '\\')
				continue;
		}

		oFilelist.push_back(filename);
		rc = unzGoToNextFile(mFileHandle);
	}

	unzCloseCurrentFile(mFileHandle);
	unzClose(mFileHandle);
	mFileHandle = NULL;

	if(rc != UNZ_END_OF_LIST_OF_FILE)
		return FilenameScanner::SS_ABORT;

	return FilenameScanner::SS_OK;
}

}

}
