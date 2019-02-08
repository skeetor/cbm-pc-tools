#include <iostream>
#include <gzguts.h>

#include "toolslib/files/GZFile.h"

namespace toolslib
{

namespace files
{

// *******************************************************************
bool GZFile::isHeader(const char *pBuffer, size_t nBufferLen)
{
	if(nBufferLen < 2)
		return false;

	const unsigned short *h = (const unsigned short *)pBuffer;
	if(*h == 0x8b1f)
		return true;

	return false;
}

// *******************************************************************
GZFile::GZFile(Filename const &oFilename)
	: super(oFilename)
{
	mFileHandle = NULL;
	mFilePos = -1;
}

GZFile::~GZFile(void)
{
	close();
}

bool GZFile::open(void)
{
	mFilePos = -1;
	super::open();
	if((mFileHandle = gzopen(getOpenpath().c_str(), getFileOpenmode().c_str())) == NULL)
		return false;

	mFilePos = 0;

	return true;
}

void GZFile::close(void)
{
	super::close();
	if(mFileHandle)
		gzclose(mFileHandle);

	mFileHandle = NULL;
	mFilePos = -1;
}

void GZFile::flush(void)
{
}

int64_t GZFile::read(void *oBuffer, int64_t nLen)
{
	if(mFileHandle == NULL || oBuffer == NULL)
		return -1;

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
		if((rd = gzread(mFileHandle, &p[total], chunk)) != chunk)
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

int64_t GZFile::write(void const *oBuffer, int64_t nLen)
{
	if(mFileHandle == NULL || oBuffer == NULL)
		return invalid64_t;

	const char *p = static_cast<const char *>(oBuffer);
	int64_t total = 0;

	while(total < nLen)
	{
		int chunk;
		if(nLen > INT_MAX)
			chunk = INT_MAX;
		else
			chunk = static_cast<int>(nLen - total);

		int rd;
		if((rd = gzwrite(mFileHandle, &p[total], chunk)) != chunk)
		{
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

int64_t GZFile::seek(int64_t nOffset, IFile::seek_pos nPos)
{
	UNUSED(nOffset);
	UNUSED(nPos);

	return gzseek64(mFileHandle, nOffset, nPos);
}

int64_t GZFile::tell(void)
{
	return gztell64(mFileHandle);
}

int64_t GZFile::length(void)
{
	// There is no way to determine the uncompressed filesize for a GZ file. The only
	// way to get this information is to read the file until the end.
	// For a more detailed explanation see:
	// http://stackoverflow.com/questions/9209138/uncompressed-file-size-using-zlibs-gzip-file-access-function

#pragma message(TOOLSLIB_WARNING "GZFile::length() not implemented!")

	return invalid64_t;
}

}

}
