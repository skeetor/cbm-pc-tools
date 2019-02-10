#ifndef _GZ_FILE_H
#define _GZ_FILE_H

#include <zlib.h>

#include "toolslib/files/BaseFile.h"

namespace toolslib
{

namespace files
{

class TOOLSLIB_API GZFile
: public virtual BaseFile
{
public:
	GZFile(Filename const &oFilename = "");
	~GZFile(void) override;

	bool open(void) override;
	void close(void) override;
	void flush(void) override;
	int64_t read(void *oBuffer, int64_t nLen) override;
	int64_t write(void const *oBuffer, int64_t nLen) override;
	int64_t seek(int64_t nOffset, IFile::seek_pos nPos) override;
	int64_t tell(void) override;
	int64_t length(void) override;

	static bool isHeader(const char *pBuffer, size_t nBufferLen);

private:
	typedef BaseFile super;

private:
	gzFile mFileHandle;
	int64_t mFilePos;		// Uncompressed position.
};

}

}

#endif // _GZ_FILE_H
