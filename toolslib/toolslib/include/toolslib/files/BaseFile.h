#ifndef _BASE_FILE_H
#define _BASE_FILE_H

#include "toolslib/toolslib_api.h"
#include "toolslib/toolslib_def.h"
#include "toolslib/files/IFile.h"

namespace toolslib
{

namespace files
{

/**
 * File handles the access to a file. A basepath can be set, in which case
 * the file class automatically prepends it to a filename. If the basepath
 * is not set (NULL), the current directory is assumed. If a full path
 * or an UNC path is used, then the basepath is ignored.
 *
 * Note that, whenever the basepath or the filname is changed and a file
 * was previously opened, it will be closed. This is also the case, when
 * the filename would be the same.
 */
class TOOLSLIB_API BaseFile
: public virtual IFile
{
public:
	BaseFile(Filename const &oFilename = "");
	~BaseFile(void) override;

	bool open(void) override;
	bool open(open_mode const &oMode) override
	{
		setOpenmode(oMode);
		open();
		return isOpen();
	}

	void close(void) override;
	bool isEOF(void) const override;

	void setFilename(Filename const &oFilename) override;
	const Filename &getFilename(void) const override;

	std::string const &getOpenpath(void) const override;

	uint64_t getFileBufferSize(void) const override;
	uint64_t setFileBufferSize(uint64_t nFileBufferSize = invalid64u_t) override;

	int getChar(void) override;

	void setOpenmode(open_mode const &oMode) override;

	/**
	 * Returns the openmode in the form of a FILE * mode.
	 */
	std::string getFileOpenmode(void) const;

	IFile::open_mode getOpenmode(void) const
	{
		return mOpenmode;
	}

	inline void setEOF(bool bEOF = true)
	{
		mEOF = bEOF;
	}

	bool isOpen(void) const override
	{
		return mIsOpen;
	}

	/**
	 * Creates all missing directories of a path for a given file. If no filename
	 * is given in the path it must end with a terminating '\' character otherwise
	 * the last directory will not be created.
	 *
	 * Example:
	 *		"d:\tmp\t0\t1\t2\t.txt"		<- creates "d:\tmp\t0\t1\t2"
	 *		"d:\tmp\t0\t1\t2\"			<- creates "d:\tmp\t0\t1\t2"
	 *		"d:\tmp\t0\t1\t2"			<- creates "d:\tmp\t0\t1" but not "t2" as a directory
	 */
	static void createPath(std::string const &oPath);

protected:
	virtual Filename &getFilename(void);
	virtual char *allocateFileBuffer(int64_t nSize);
	virtual void freeFileBuffer(char *oFileBuffer);
	char *getFileBuffer(void) const override;
	inline void setFileBuffer(char *oFileBuffer)
	{
		mFileBuffer = oFileBuffer;
	}

	void setIsOpen(bool bIsOpen = true)
	{
		mIsOpen = bIsOpen;
	}

private:
	char *mFileBuffer;
	uint64_t mFileBufferSize;
	Filename mFilename;
	IFile::open_mode mOpenmode;
	bool mEOF:1;
	bool mIsOpen:1;
};

}

}

#endif // _BASE_FILE_H
