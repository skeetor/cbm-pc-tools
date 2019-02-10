#ifndef _MEMORYFILE_H
#define _MEMORYFILE_H

#include <vector>

#include "toolslib/files/BaseFile.h"

namespace toolslib
{

namespace files
{

/**
 * Memoryfile stores a file simply in memory, so it can be used for fast access
 * with the same interface but no disk io.
 */
class TOOLSLIB_API MemoryFile
: public virtual BaseFile
{
public:
	MemoryFile(Filename const &oFilename = "");
	~MemoryFile(void) override;

	bool open(void) override;
	void close(void) override;
	void flush(void) override;
	bool isEOF(void) const override;
	int64_t read(void *oBuffer, int64_t nLen) override;
	int64_t write(const void *oBuffer, int64_t nLen) override;
	int64_t seek(int64_t nOffset, IFile::seek_pos nPos) override;
	int64_t tell(void) override;
	int64_t length(void) override;
	int getChar(void) override;

public:
	/**
	 * Reserve memory, for the intended filesize, but doesn't change it yet,
	 * Usuefull for avoiding reallocations.
	 */
	void reserve(int64_t nLen);

	/**
	 * Directly set the length of the file.
	 */
	void resize(int64_t nLen);

private:
	typedef BaseFile super;

private:
	std::vector<uint8_t> mFileMem;
	int64_t mFilePos;
	int64_t mEndPos;
};

}

}

#endif // _MEMORYFILE_H
