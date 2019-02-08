#ifndef _IFILE_H
#define _IFILE_H

#include <cstdio>
#include <memory>

#include "toolslib/toolslib_api.h"
#include "toolslib/toolslib_def.h"
#include "toolslib/files/Filename.h"

namespace toolslib
{

namespace files
{

class TOOLSLIB_API IFile
{
public:
	typedef enum
	{
		set = 0,			// Seek to an absolute position from the start of the file.
		cur = 1,			// Seek forward/backward from the current position
		end = 2				// Seek to an absolute position from the end of the file.
	} seek_pos;

public:
	class deleter
	{
	public:
		void operator()(IFile *pPointer)
		{
			if(pPointer)		// Pointer is never NULL here, but just to be sure...
				pPointer->close();
		}
	};

	class Ptr : public std::unique_ptr<IFile, deleter>
	{
	public:
		typedef std::unique_ptr<IFile, deleter> ptr_type;

	public:
		Ptr(IFile *pPointer) : ptr_type(pPointer, deleter())
		{
		}
	};

	class open_mode
	{
		public:
			bool binary;
			bool read;
			bool write;
			bool append;
			bool create;
			bool truncate;

			static open_mode init(void)
			{
				open_mode md;

				md.binary = false;
				md.read = false;
				md.write = false;
				md.append = false;
				md.create = false;
				md.truncate = false;

				return md;
			}
	};

	static const open_mode open_default;

public:
	IFile(void) {};
	virtual ~IFile(void) {};

	virtual bool open(void) = 0;
	virtual bool open(open_mode const &oMode) = 0;

	virtual void close(void) = 0;
	virtual void flush(void) = 0;
	virtual bool isEOF(void) const = 0;
	virtual int64_t read(void *oBuffer, int64_t nLen) = 0;
	virtual int64_t write(void const *oBuffer, int64_t nLen) = 0;
	virtual int64_t seek(int64_t nOffset, IFile::seek_pos nPos) = 0;
	virtual int64_t tell(void) = 0;
	virtual int64_t length(void) = 0;
	virtual int getChar(void) = 0;

	virtual void setFilename(Filename const &oFilename) = 0;
	virtual const Filename &getFilename(void) const = 0;

	/**
	 * getPath() returns the full pathname to the file.
	 * This function will always return a valid pointer, so it 
	 * can be safely used. However, if the provided paths are
	 * invalid, the result from getOpenpath() may not be a valid
	 * path either, so the open call() must still be checked for success.
	 */
	virtual std::string const &getOpenpath(void) const = 0;

	/**
	 * FileBufferSize determines the buffersize for the input stream.
	 * If the buffersize == -1 the object should propose a suitable
	 * buffersize.
	 * This value must be set, before open() is called
	 * otherwise it may not be applied. If the file is already opened,
	 * the new value may be ignored.
	 */
	virtual uint64_t getFileBufferSize(void) const = 0;
	virtual uint64_t setFileBufferSize(uint64_t nFileBufferSize = invalid64u_t) = 0;

	/**
	 * getFileBuffer() ruins the privacy of the file object, but for performance
	 * reasons, we nevertheless allow access here. However, the client should be
	 * aware that there may be implementations which don't even have an internal
	 * buffer and NULL can be returned. If a pointer is returned, it must be
	 * guaruanteed that it is not smaller than getFileBufferSize() returns.
	 */
	virtual char *getFileBuffer(void) const = 0;

	virtual void setOpenmode(open_mode const &oMode) = 0;

	size_t printf(const char *oFormat, ...);
};

}

}

#endif // _IFILE_H
