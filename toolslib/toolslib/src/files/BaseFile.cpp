#include <Windows.h>
#include <direct.h>
#include <iostream>

#include "toolslib/files/BaseFile.h"

namespace toolslib
{

namespace files
{

using namespace std;

BaseFile::BaseFile(Filename const &oFilename)
{
	mEOF = false;
	mFileBuffer = NULL;
	mFileBufferSize = invalid64u_t;
	mOpenmode = IFile::open_default;
	setFilename(oFilename);
}

BaseFile::~BaseFile(void)
{
	freeFileBuffer(mFileBuffer);
}

bool BaseFile::isEOF(void) const
{
	return mEOF;
}

void BaseFile::setFilename(Filename const &oFilename)
{
	mFilename = oFilename;
}

const Filename &BaseFile::getFilename(void) const
{
	return mFilename;
}

Filename &BaseFile::getFilename(void)
{
	return mFilename;
}

string const &BaseFile::getOpenpath(void) const
{
	return mFilename.getOpenpath();
}

bool BaseFile::open(void)
{
	close();
	setEOF(false);
	allocateFileBuffer(setFileBufferSize());
	setIsOpen(true);

	return true;
}

void BaseFile::close(void)
{
	flush();
	setEOF(false);
	setIsOpen(false);

	freeFileBuffer(mFileBuffer);
}

void BaseFile::setOpenmode(IFile::open_mode const &oOpenmode)
{
	mOpenmode = oOpenmode;
}

string BaseFile::getFileOpenmode(void) const
{
	string md;

	if(mOpenmode.read && mOpenmode.write)
	{
		if(mOpenmode.truncate)
			md += "w+";
		else
			md += "r+";
	}
	else
	{
		if(mOpenmode.read)
			md += "r";
		else
		{
			md += "w";
			if(mOpenmode.truncate)
				md += "+";
		}
	}

	if(mOpenmode.append)
		md += "a";

	if(mOpenmode.binary)
		md += "b";

	return md;
}

char *BaseFile::allocateFileBuffer(int64_t nSize)
{
	if(!mFileBuffer)
	{
		mFileBuffer = new char[(size_t)nSize+32];
		memset(mFileBuffer, 0xFE, (size_t)(nSize+32));
	}

	return mFileBuffer;
}

void BaseFile::freeFileBuffer(char *oFileBuffer)
{
	UNUSED(oFileBuffer);

	if(mFileBuffer)
		delete [] mFileBuffer;

	mFileBuffer = NULL;
}

char *BaseFile::getFileBuffer(void) const
{
	return mFileBuffer;
}

uint64_t BaseFile::getFileBufferSize(void) const
{
	return mFileBufferSize;
}

uint64_t BaseFile::setFileBufferSize(uint64_t nFileBufferSize)
{
	if (nFileBufferSize == invalid64u_t)
		nFileBufferSize = 4096u;

	mFileBufferSize = nFileBufferSize;

	return mFileBufferSize;
}

int BaseFile::getChar(void)
{
	char c;
	int64_t rd;

	// Not the best idea to use this implementation but for completeness it is provided anyway.
	// If getch() functionality is needed, a buffered stream object is the better solution.
	if((rd = read(&c, 1)) < 0)
		return 0;

	return c;
}

void BaseFile::createPath(string const &oPath)
{
	string p;
	size_t i = 0;

	while(i < oPath.length())
	{
		size_t pos = oPath.find('\\', i);
		if(pos == string::npos)
			return;

		p = oPath.substr(0, pos);
		i = pos+1;

		_mkdir(p.c_str());
	}
}

}

}
