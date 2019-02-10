
#include "toolslib/files/MemoryFile.h"

namespace toolslib
{

namespace files
{

using namespace std;

MemoryFile::MemoryFile(Filename const &oFilename)
	: super(oFilename)
	, mFilePos(-1)
	, mEndPos(0)
{
}

MemoryFile::~MemoryFile(void)
{
	close();
}

void MemoryFile::reserve(int64_t nLen)
{
	mFileMem.reserve((size_t)nLen);
}

void MemoryFile::resize(int64_t nLen)
{
	mFileMem.resize((size_t)nLen);
}

bool MemoryFile::open(void)
{
	super::open();
	mFilePos = 0;

	IFile::open_mode md = getOpenmode();
	if (md.truncate)
		mFileMem.resize(0);

	return true;
}

void MemoryFile::close(void)
{
	flush();
	super::close();
	mFilePos = invalid64_t;
	mEndPos = 0;
}

void MemoryFile::flush(void)
{
}

bool MemoryFile::isEOF(void) const
{
	if (mFilePos < 0)
		return true;

	if (!isOpen())
		return true;

	if ((size_t)(mFilePos+mEndPos) > mFileMem.size())
		return true;

	return false;
}

int64_t MemoryFile::seek(int64_t nOffset, IFile::seek_pos nPos)
{
	if (!isOpen())
		return invalid64_t;

	switch(nPos)
	{
		case IFile::cur:
		{
			mFilePos += nOffset;
			mEndPos = 0;
			if (!isEOF())
				return mFilePos;

			if (mFilePos >= 0)
			{
				if ((size_t)mFilePos > mFileMem.size())
					mEndPos = 1;

				mFilePos = mFileMem.size();
			}
			else
				mFilePos = 0;
		}
		break;

		case IFile::set:
		{
			mFilePos = nOffset;
			mEndPos = 0;
			if (!isEOF())
				return mFilePos;

			if (nOffset >= 0)
			{
				if ((size_t)mFilePos > mFileMem.size())
					mEndPos = 1;

				mFilePos = mFileMem.size();
			}
			else
				mFilePos = 0;
		}
		break;

		case IFile::end:
		{
			mFilePos = mFileMem.size() - nOffset;
			mEndPos = 0;
			if (mFilePos >= 0)
				return mFilePos;

			mFilePos = 0;
		}
		break;
	}

	return invalid64_t;
}

int64_t MemoryFile::tell(void)
{
	if (!isOpen())
		return invalid64_t;

	return mFilePos;
}

int64_t MemoryFile::length(void)
{
	if(isOpen())
		return mFileMem.size();

	return invalid64_t;
}

int64_t MemoryFile::read(void *oBuffer, int64_t nLen)
{
	if (!isOpen())
		return invalid64_t;

	uint64_t end = (uint64_t)nLen;

	int64_t pos = mFilePos;
	mFilePos += end;

	if ((size_t)mFilePos > mFileMem.size())
	{
		mFilePos = mFileMem.size();
		mEndPos = 1;
		end = mFileMem.size() - pos;
	}
	else
		mEndPos = 0;

	if (end <= 0)
		return 0;

	memcpy(oBuffer, &mFileMem[(size_t)pos], (size_t)end);

	return end;
}

int64_t MemoryFile::write(const void *oBuffer, int64_t nLen)
{
	if (!isOpen())
		return invalid64_t;

	IFile::open_mode md = getOpenmode();
	if(!md.write)
		return invalid64_t;

	if (md.append)
		seek(0, end);

	uint64_t end = (uint64_t)nLen;

	if ((size_t)mFilePos + end > mFileMem.size())
		mFileMem.resize((size_t)(mFilePos + end));

	memcpy(&mFileMem[(size_t)mFilePos], oBuffer, (size_t)end);
	mFilePos += end;
	mEndPos = 0;

	return end;
}

int MemoryFile::getChar(void)
{
	if (!isOpen())
		return invalid64_t;

	if(!isEOF())
		return (int)mFileMem[(size_t)mFilePos];

	return 0;
}

}

}
