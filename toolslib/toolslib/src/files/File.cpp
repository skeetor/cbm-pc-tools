#define _CRT_SECURE_NO_WARNINGS

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include "toolslib/files/File.h"

namespace toolslib
{

namespace files
{

using namespace std;

File::File(Filename const &oFilename)
	: super(oFilename)
	, mDoClose(false)
{
	init();
}

File::File(Filename const &oFilename, FILE *oFile, const IFile::open_mode &mode, bool bDoClose)
{
	if (oFile)
	{
		setFilename(oFilename);
		setOpenmode(mode);
		setEOF(false);
		allocateFileBuffer(setFileBufferSize());
		setIsOpen(true);
		mDoClose = bDoClose;
	}
	else
		init();

	mFileHandle = oFile;
}

void File::init(void)
{
	mDoClose = false;
	mFileHandle = NULL;
	IFile::open_mode md = { false, true };	// read only

	setOpenmode(md);
}

File::~File(void)
{
	super::close();
}

FILE *File::getFileHandle(void) const
{
	return mFileHandle;
}

IFile::open_mode File::openMode(string const &oMode)
{
	IFile::open_mode md = IFile::open_mode::init();

	for(string::const_iterator it = oMode.begin(); it != oMode.end(); ++it)
	{
		char const &c = *it;

		switch(c)
		{
			case 'b':
				md.binary = true;
			break;

			case '+':
			{
				md.read = true;
				md.write = true;
			}
			break;

			case 'r':
				md.read = true;
			break;

			case 'w':
			{
				md.write = true;
				md.truncate = true;
				md.create = true;
			}
			break;
		}
	}

	return md;
}

bool File::open(void)
{
	super::open();
	mFileHandle = fopen(getOpenpath().c_str(), getFileOpenmode().c_str());
	if (mFileHandle == NULL)
	{
		setIsOpen(false);
		return false;
	}

	setvbuf(mFileHandle, getFileBuffer(), _IOFBF, (size_t)getFileBufferSize());

	setIsOpen(true);
	return true;
}

void File::close(void)
{
	if (mDoClose)
	{
		super::close();

		if (mFileHandle)
			fclose(mFileHandle);

		mFileHandle = NULL;
	}
	else
		flush();
}

void File::flush(void)
{
	fflush(mFileHandle);
}

bool File::isEOF(void) const
{
	return feof(mFileHandle) != 0;
}

int64_t File::seek(int64_t nOffset, IFile::seek_pos nPos)
{
	// Probably needs a rework if the offset is longer than LONG_MAX because
	// then it might behave wrong.
	int mode = SEEK_SET;
	switch(nPos)
	{
		case IFile::cur:
			mode = SEEK_CUR;
		break;

		case IFile::set:
			mode = SEEK_SET;
		break;

		case IFile::end:
			mode = SEEK_END;
		break;
	}

	return _fseeki64(mFileHandle, nOffset, mode);
}

int64_t File::tell(void)
{
	return _ftelli64(mFileHandle);
}

int64_t File::length(void)
{
	if(!mFileHandle)
		return invalid64_t;

	struct _stati64 buffer;

	int rc = _stat64(getOpenpath().c_str(), &buffer);
	return rc == 0 ? buffer.st_size : invalid64_t;
}

int64_t File::read(void *oBuffer, int64_t nLen)
{
	return fread(oBuffer, 1, (size_t)nLen, mFileHandle);
}

int64_t File::write(void const *oBuffer, int64_t nLen)
{
	return fwrite(oBuffer, 1, (size_t)nLen, mFileHandle);
}

int File::getChar(void)
{
	return fgetc(mFileHandle);
}

}

}
