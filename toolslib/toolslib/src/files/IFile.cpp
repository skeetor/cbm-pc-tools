#define _SCL_SECURE_NO_WARNINGS

#include <iostream>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "toolslib/files/IFile.h"
#include "toolslib/files/FileFactory.h"

namespace toolslib
{

namespace files
{

//											   binary	read	write	append	create  truncate
const IFile::open_mode IFile::open_default = { true,	true,	false,	false,	false,   false };

size_t IFile::printf(const char *oFormat, ...)
{
	char buffer[4096];
	char *p = buffer;
	va_list   ap;
	va_start(ap, oFormat);

	size_t len = vsnprintf(p, sizeof(buffer)-1, oFormat, ap);
	if(len == -1)
	{
		len = _vscprintf(oFormat, ap);
		p = new char[len+1];
		vsnprintf(p, len, oFormat, ap);
	}

	va_end(ap);

	len = (size_t)write(p, len);

	if(p != buffer)
		delete [] p;

	return len;
}

}

}
