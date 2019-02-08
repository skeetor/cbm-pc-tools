#include <windows.h>

#include <iostream>

#include "toolslib/files/FilesystemScanner.h"

namespace toolslib
{

namespace files
{

using namespace std;

// ******************************************************************************
FilenameScanner::FilenameScanner(Filename const &oRoot, bool bIncludeSubdirectories)
{
	mRoot = oRoot;
	mPattern = mRoot.getFilename();

	includeSubdirectories(bIncludeSubdirectories);
}

vector<string> FilenameScanner::scan(void)
{
	scan(mRoot);
	return mFiles;
}

// ******************************************************************************
FilesystemScanner::FilesystemScanner(Filename const &oRoot, bool bIncludeSubdirectories)
	: super(oRoot, bIncludeSubdirectories)
{
}

FilenameScanner::ScanState FilesystemScanner::scan(Filename const &oRoot)
{
	HANDLE hFind;
	WIN32_FIND_DATA fd;
	Filename root = oRoot;
	root.normalize();
	string dir = root.getBasePath();
	ScanState state = SS_OK;

	if(root.getFilename() == "")
		root.setFilename("*");

	if((hFind = FindFirstFile((dir+"*").c_str(), &fd)) != INVALID_HANDLE_VALUE)
	{
		do
		{
			bool isDir = (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			state = SS_OK;

			// If the path is '.' or '..' we ignore it.
			if(fd.cFileName[0] == '.')
			{
				if(fd.cFileName[1] == 0)
					continue;

				if(fd.cFileName[1] == '.' && fd.cFileName[2] == 0)
					continue;
			}

			if(isDir && includeSubdirectories())// If it is a directory we have to enter it on request
				state = scan(dir + fd.cFileName+'\\'+root.getFilename());
			else
			{
				string const &p = fd.cFileName;
				if(Filename::matchesWildcard(Pattern().c_str(), p.c_str(), p.c_str()+p.size(), false, false))
					Files().push_back(dir+p);
			}
		} while (state != SS_ABORT && FindNextFile(hFind, &fd));

		FindClose(hFind);
	}

	return state;
}

}

}
