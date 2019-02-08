#ifndef _FILE_FACTORY_H
#define _FILE_FACTORY_H

#include <cstdio>

#include "toolslib/files/IFile.h"
#include "toolslib/files/FilesystemScanner.h"

namespace toolslib
{

namespace files
{

/**
 * FileFactory provides a common interface for identifying
 * a file based on certain characteristics, and returns an appropriate
 * file or stream object. The factory can also be used to create a specifc
 * file object by the type.
 *
 * Note that objectes created by the factory, should be deleted by the
 * appropriate calls
 */
class TOOLSLIB_API FileFactory
{
public:
	/**
	 * Define the filetypes which are supported by the factory.
	 * Note, that, if this is changed, the filetypes in cfile.h must also be adpated.
	 */
	typedef enum
	{
		FF_UNKNOWN,				// Undefined
		FF_DIRECT,				// file with file handle	/ read-write
		FF_FILE,				// file with FILE			/ read-write
		FF_GZ,					// GZ compressed file	/ read only
		FF_ZIP,					// ZIP compressed file	/ read only
		FF_RLE,					// RLE compressed file	/ not implemented
		FF_LZ4,					// LZ4 compressed file	/ read-write

		FF_MAX
	} FileType;

	typedef struct 
	{
		std::string Name;			// Extension name
		FileType Type;				// Factory type
		IFile::open_mode Mode;		// Supported mode (read, write, etc.)
	} file_type_mapping;

public:
	FileFactory();
	virtual ~FileFactory(void);

	static FileFactory *getInstance(void);

	/**
	 * getFile() returns an appropriate File object for the specified filetype.
	 * The file is not opened yet, so the caller is responsible for
	 * opening it before using the stream.
	 *
	 * This function will return exactly the specified object, no detection is done.
	 */
	IFile *createFileInstance(Filename const &oFilename, FileType oFileType) const;

	/**
	 * Return the file object for the given filename. If FileType is set, the object 
	 * is returned, otherwise the object type is determined automatically.
	 * The file is not opened.
	 */
	IFile *getFile(Filename const &oFilename, FileType oFileType = FileFactory::FF_UNKNOWN) const;

	/**
	 * Identifies and opens the specified file. If the file is not known or can not be opened NULL is returned.
	 */
	IFile *openFile(Filename const &oFilename, IFile::open_mode const &oOpenMode = IFile::open_default, FileType oFileType = FileFactory::FF_UNKNOWN) const;

	void closeFile(IFile *&pFile)
	{
		if(pFile)
			pFile->close();

		delete pFile;
		pFile = NULL;
	}

	/**
	 * getScanner() returns an appropriate scanner object for the given path.
	 */
	FilenameScanner *getScanner(Filename const &oFilename, bool bIncludeSubdirectories = true) const;

	/**
	 * Returns the index of the container if the filename is part of a container.
	 * If it is not supported, then -1 is returned.
	 *
	 * If the container is found, then path of the container is returned in ContainerName.
	 */
	size_t containerFilename(Filename const &oFilename, std::string &oContainerName) const;

protected:
	/**
	 * Detect the filetype. Depending on the file it can happen that the 
	 * filename is modified, so the returned filename should always be used
	 * instead of the one passed in.
	 */
	Filename detectFileType(Filename const &oFilename, FileType &oType) const;

private:
	static FileFactory *mInstance;			// Singleton instance.
};

}

}

#endif // _FILE_FACTORY_H
