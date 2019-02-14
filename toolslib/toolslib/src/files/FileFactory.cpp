#include <algorithm>
#include <vector>

#include "toolslib/files/FileFactory.h"

#include "toolslib/files/File.h"
#include "toolslib/files/GZFile.h"
#include "toolslib/files/ZIPFile.h"
#include "toolslib/files/ZIPScanner.h"

#include <toolslib/string/Wildcards.h>
#include <toolslib/string/Helpers.h>

namespace toolslib
{

namespace files
{

using namespace std;
using namespace toolslib;
using namespace toolslib::strings;

static vector<string> gSupportedFiles;
static vector<FileFactory::FileType> gSupportedFileTypes;

FileFactory *FileFactory::mInstance = NULL;

FileFactory::FileFactory(void)
{
}

FileFactory::~FileFactory(void)
{
}

FileFactory *FileFactory::getInstance(void)
{
	if(!mInstance)
	{
		gSupportedFiles.push_back(".GZ");
		gSupportedFiles.push_back(".ZIP");

		gSupportedFileTypes.push_back(FileFactory::FF_GZ);
		gSupportedFileTypes.push_back(FileFactory::FF_ZIP);

		mInstance = new FileFactory();
	}

	return mInstance;
}

size_t FileFactory::containerFilename(Filename const &oFilename, string &oContainerName) const
{
	string fn = oFilename.getOpenpath();
	string fnup = fn;									// preserve the case, just to be sure.
	toUpperStr(fnup);
	size_t pos = string::npos;
	size_t i = findFirstSubstring(fnup, gSupportedFiles, pos);

	if(i == -1)		// Unknown container
		return i;

	// We have a hit. it can be in the middle of the string, if it is a container
	// like FILENAME.ZIP/directory/file.txt
	// or it can be the file itself like FILENAME.ZIP
	pos += gSupportedFiles[i].size();
	if(pos <= fnup.size())
		oContainerName.assign(fn.begin(), fn.begin()+pos);

	return i;
}

/**
 * detectFileType tries to determine the filetype based on the filename.
 * First we check for files which are containers (like ZIP). If this is
 * not the case, we check the filename itself.
 */
Filename FileFactory::detectFileType(Filename const &oFilename, FileFactory::FileType &nType) const
{
	nType = FileFactory::FF_UNKNOWN;
	Filename fn = oFilename;
	string cn;
	size_t i = containerFilename(oFilename, cn);

	if(i == -1)		// Unknown Filetype
		return fn;

	nType = gSupportedFileTypes[i];

	string f = oFilename.getOpenpath();
	if(f.size() > cn.size())
	{
		// Archive is the directory and the filename is the path inside the archive.
		fn.setBasePath(cn);
		fn.setFilename(f.substr(cn.size()+1));
	}
	else
	{
		// Archive is the filename itself.
		fn.setBasePath("");
		fn.setFilename(f);
		fn.normalize();
	}

	return fn;
}

IFile *FileFactory::createFileInstance(Filename const &oFilename, FileType oFileType) const
{
	IFile *fl = NULL;

	switch(oFileType)
	{
		case FileFactory::FF_FILE:
			fl = new File(oFilename);
		break;

		case FileFactory::FF_GZ:
			fl = new GZFile(oFilename);
		break;

		case FileFactory::FF_ZIP:
			fl = new ZipFile(oFilename);
		break;
	}

	return fl;
}

IFile *FileFactory::getFile(Filename const &oFilename, FileType oFileType, FileType oDefault) const
{
	FileFactory::FileType type;
	IFile *fl = NULL;
	Filename fn = oFilename;
	if(oFileType == FileFactory::FF_UNKNOWN)
		fn = detectFileType(oFilename, type);
	else
		type = oFileType;

	if (type == FileFactory::FF_UNKNOWN)
		type = oDefault;

	fl = createFileInstance(fn, type);

	return fl;
}

IFile *FileFactory::openFile(Filename const &oFilename, IFile::open_mode const &oOpenMode, FileType oFileType) const
{
	IFile *fl = getFile(oFilename, oFileType);
	if(!fl)
		return NULL;

	fl->setOpenmode(oOpenMode);
	if(!fl->open())
	{
		delete fl;
		return NULL;
	}

	return fl;
}

FilenameScanner *FileFactory::getScanner(Filename const &oFilename, bool bIncludeSubdirectories) const
{
	FileFactory::FileType type;
	Filename fn = oFilename;
	fn.normalize();

	Filename archive = detectFileType(fn.getBasePath(), type);

	// Pattern is part of a archive.
	if(archive.getBaseDir().length() > 0)
	{
		// Determine the type of the archive, so we can deliver the appropriate scanner
		detectFileType(archive.getBasePath(), type);
	}
	else
		archive.normalize();

	// Append the filename back to the part of the directory inside the archive.
	archive.setFilename(archive.getFilename()+fn.getFilename());

	FilenameScanner *sc = NULL;

	switch(type)
	{
		case FileFactory::FF_DIRECT:
		case FileFactory::FF_FILE:
			sc = new FilesystemScanner(archive, bIncludeSubdirectories);
		break;

		case FileFactory::FF_ZIP:
			sc = new ZIPScanner(archive, bIncludeSubdirectories);
		break;

		case FileFactory::FF_GZ:
		case FileFactory::FF_RLE:
			sc = NULL;
		break;
	}

	return sc;
}

}

}
