#include <algorithm>
#include <iostream>

#include "toolslib/files/ZIPFile.h"
#include "toolslib/string/Helpers.h"

#define CASESENSITIVITY (0)

namespace toolslib
{

namespace files
{

using namespace std;
using namespace toolslib;
using namespace toolslib::strings;

// *******************************************************************
bool ZipFile::isHeader(const char *pBuffer, size_t nBufferLen)
{
	if(nBufferLen < 4)
		return false;

	const unsigned int *h = (const unsigned int *)pBuffer;
	if(*h == 0x04034b50)
		return true;

	return false;
}

// *******************************************************************
// For a ZIP file we must process the filename first so we call the default constructor
// here, and set the filename manually.
ZipFile::ZipFile(Filename const &oFilename)
	: super()
{
	mFileHandle = NULL;
	mFilePos = invalid64_t;
	mFileSize = invalid64u_t;
	setDefaultExtension(".txt");
	setFilename(oFilename);
}

ZipFile::~ZipFile(void)
{
	close();
}

void ZipFile::setFilename(Filename const &oFilename)
{
	Filename f = oFilename;
	string fnup = f.getOpenpath();
	transform(fnup.begin(), fnup.end(), fnup.begin(), toUpper);

	// If we don't find the ZIP extension, it will result in an error when opening
	// but the client has to deal with it. If the extension of the zip file should be
	// something (arbitrary) other than "ZIP", then the code would have to be adapted here.
	string ext = ".ZIP";
	size_t pos = fnup.find(ext);
	if(pos != string::npos)
	{
		string fn = f.getOpenpath();  // original unmodified filename (preserves the case).
		pos += ext.length();

		f.setBasePath(fn.substr(0, pos));
		if(fn.size() > pos)				// The filename contains a path to a file inside the archive
			f.setFilename(fn.substr(pos+1, fn.size()));
		else							// The filename is only the archive, so we will open the first file we find inside it.
			f.setFilename("");
	}

	super::setFilename(f);
}

bool ZipFile::selectFile(string const &oFilename)
{
	// Change the filename part only, so that the base directory stays the ZIP file itself.
	Filename &fn = getFilename();
	fn.setFilename(oFilename);

	if(mFileHandle == NULL)
		return false;

	unzCloseCurrentFile(mFileHandle);

	// Locate the file in the ZIP to make it the current file.
	if (unzLocateFile(mFileHandle, oFilename.c_str(), CASESENSITIVITY) != UNZ_OK)
	{
		//cerr << "Unable to open " << getFilename().getFileDir() << endl;
		return false;
	}

	return true;
}

bool ZipFile::open(void)
{
	mFilePos = invalid64_t;
	mFileSize = invalid64u_t;

	super::open();
	Filename &f = getFilename();

	// The base path must contain only the path to the ZIP file including the file itself.
	// i.E. d:\tmp\file.zip
	if((mFileHandle = unzOpen64(f.getBaseDir().c_str())) == NULL)
	{
		//cerr << "Unable to open " << getFilename().getBaseDir() << endl;
		return false;
	}

	// A ZIP file uses internally the '/' instead of '\' like in DOS world,
	// so we have to make sure that the filename is converted.
	string fn = f.getFilename();
	if(fn.length() > 0)
		replace(fn.begin(), fn.end(), '\\', '/');
	else
	{
		// If the filename is empty we simply take the first file we find
		// from the archive.
		unz_file_info64 file_info;
		char filename[2001];
		if(unzGetCurrentFileInfo64(mFileHandle, &file_info, filename, sizeof(filename), NULL, NULL, NULL, NULL) != UNZ_OK)
			return false;

		mFileSize = file_info.uncompressed_size;

		fn = filename;
		if(fn.size() > 0 && fn[fn.size()-1] == '/')
			return false;
	}

	if(!selectFile(fn))
		return false;

	// Open the current file inside the ZIP
	if(unzOpenCurrentFile(mFileHandle) != UNZ_OK)
		return false;

	mFilePos = 0;

	return true;
}

void ZipFile::close(void)
{
	super::close();
	if(mFileHandle)
	{
		// Close the current file
		unzCloseCurrentFile(mFileHandle);
		unzClose(mFileHandle);
	}

	mFileHandle = NULL;
	mFilePos = invalid64_t;
}

void ZipFile::flush(void)
{
}

int64_t ZipFile::read(void *oBuffer, int64_t nLen)
{
	if(mFileHandle == NULL || oBuffer == NULL)
		return invalid64_t;

	char *p = static_cast<char *>(oBuffer);
	int64_t total = 0;
	while(total < nLen)
	{
		int chunk;
		if(nLen > INT_MAX)
			chunk = INT_MAX;
		else
			chunk = static_cast<int>(nLen - total);

		int rd;
		setEOF(false);
		if((rd = unzReadCurrentFile(mFileHandle, &p[total], chunk)) != chunk)
		{
			if(!rd)
				setEOF();

			if(rd < 0)
				return invalid64_t;

			total += rd;
			mFilePos += total;
			return total;		// If 0 ==> EOF
		}

		total += rd;
	}

	mFilePos += total;
	return total;
}

int64_t ZipFile::write(void const *oBuffer, int64_t nLen)
{
	UNUSED(nLen);

	if(mFileHandle == NULL || oBuffer == NULL)
		return invalid64_t;

#pragma message(TOOLSLIB_WARNING "ZipFile::write() not implemented!")
	cerr << "Write operation not implemented for ZIP archives" << endl;

	return invalid64_t;
}

int64_t ZipFile::seek(int64_t nOffset, IFile::seek_pos nPos)
{
	UNUSED(nOffset);
	UNUSED(nPos);

#pragma message(TOOLSLIB_WARNING "ZipFile::seek() not implemented!")
	cerr << "seek operation not implemented for ZIP archives" << endl;

	return invalid64_t;
}

int64_t ZipFile::tell(void)
{
	return mFilePos;
}

int64_t ZipFile::length(void)
{
	int64_t rc = mFileSize;
	if(rc == invalid64_t)
	{
		// The file has to be opened in order for getting the filesize.
		// If the file is already opened and don't have a filesize then something
		// is wrong anyway.
		if(mFileHandle == NULL && open())
		{
			rc = mFileSize;
			close();
		}
	}

	return rc;
}

}

}
