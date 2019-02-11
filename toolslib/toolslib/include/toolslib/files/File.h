#ifndef _FILE_H
#define _FILE_H

#include <cstdio>

#include "toolslib/files/BaseFile.h"

namespace toolslib
{

namespace files
{

/**
 * File handles the access to a file.
 * Note that, whenever the basepath or the filname is changed and a file
 * was previously opened, it will be closed. This is also the case, when
 * the filename would be the same.
 */
class TOOLSLIB_API File
: public virtual BaseFile
{
public:
	File(Filename const &oFilename = "");

	/**
	 * Take ownership of the specified file. The caller is responsible to
	 * set the appropriate open mode for proper functionality.
	 * If DotClose is false, the file will stay open after a close() operation
	 * or destruction.
	 */
	File(FILE *oFile, const IFile::open_mode &mode, bool bDoClose = true);
	~File(void) override;

	bool open(void) override;
	void close(void) override;
	void flush(void) override;
	bool isEOF(void) const override;
	int64_t read(void *oBuffer, int64_t nLen) override;
	int64_t write(void const *oBuffer, int64_t nLen) override;
	int64_t seek(int64_t nOffset, IFile::seek_pos nPos) override;
	int64_t tell(void) override;
	int64_t length(void) override;
	int getChar(void) override;

	/**
	 * Convert the fopen() mode string to the internal mode.
	 */
	static IFile::open_mode openMode(std::string const &oMode);

protected:
	virtual FILE *getFileHandle(void) const;

private:
	typedef BaseFile super;

	void init(void);

private:
	FILE *mFileHandle;
	bool mDoClose;
};

}

}

#endif // _FILE_H
