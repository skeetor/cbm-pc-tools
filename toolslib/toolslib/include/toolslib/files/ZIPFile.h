#ifndef _ZIP_FILE_H
#define _ZIP_FILE_H

#include <unzip.h>

#include "toolslib/files/BaseFile.h"

namespace toolslib
{

namespace files
{

/**
 * ZipFile handles access to ZIP compressed files.
 * Since a ZIP file can contain multiple files and directories there are various methods
 * how to access a particular file inside the archive.
 *
 * 1. The simplest way to access a single file inside the archive is to pass the full path
 *    as the filename. This includes the ZIP file itself as well.
 *       Example:  D:\tmp\my.zip\dir_inside\filename.txt
 *    In this case read will read the file just like any other ordinary file
 *
 * 2. Specify a directory (inside the archive). Subsequent operations on a file will use this
 *    as a base.
 *       Example: d:\tmp\my.zip\dir_inside
 *    In this case the target file must be set with selectFile("filename.txt") and then open()
 *    can be used. The filename can also contain directories where the archive is the root.
 *        Example: d:\tmp\my.zip
 *                 selectFilename("dir_inside\my_file.txt")
 *
 * 3. Specify only the ZIP file itself. In this case the directory (if appropriate) and the
 *    filename must be set with selectFilename() before a call to open.
 *    If the directory and filename is empty and open() is called, then it is assumed that
 *    a file exists in the archive with the same name as the archive and the default extension.
 *
 * NOTE:
 *     1. write() is not supported yet.
 *     2. The ZIP file uses internally always '/' as a directory separator but this is translated
 *     automatically, so the client doesn't need to worry about back- or forwardslashes and they
 *     even can be mixed.
 */
class TOOLSLIB_API ZipFile
: public virtual BaseFile
{
public:
	ZipFile(Filename const &oFilename = "");
	~ZipFile(void) override;

	bool open(void) override;
	void close(void) override;
	void flush(void) override;
	int64_t read(void *oBuffer, int64_t nLen) override;
	int64_t write(void const *oBuffer, int64_t nLen) override;
	int64_t seek(int64_t nOffset, IFile::seek_pos nPos) override;
	int64_t tell(void) override;
	int64_t length(void) override;

	void setFilename(Filename const &oFilename) override;

	inline std::string getDefaultExtension(void)
	{
		return mDefaultExtension;
	}

	inline void setDefaultExtension(std::string const &oDefaultExtension)
	{
		mDefaultExtension = oDefaultExtension;
	}

	// The filename must only reference a file inside the archive (which can contain
	// a path as well). 
	bool selectFile(std::string const &oFilename);

	inline std::string getSelectedFile(void)
	{
		return getFilename().getFilename();
	}

	static bool isHeader(const char *pBuffer, size_t nBufferLen);

private:
	typedef BaseFile super;

private:
	unzFile mFileHandle;
	int64_t mFilePos;		// Uncompressed position.
	uint64_t mFileSize;		// Uncompressed size.
	std::string mDefaultExtension;
};

}

}

#endif // _ZIP_FILE_H
