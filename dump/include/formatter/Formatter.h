#ifndef DUMP_FORMATTER_H
#define DUMP_FORMATTER_H

#include <string>

#include "toolslib/toolslib_api.h"
#include "toolslib/files/IFile.h"

class Formatter
{
public:
	virtual ~Formatter() {}

	/**
	 * Formats the input data into the output data. If flush is true, the formatter
	 * should flush everything as the formatter will no longer be called.
	 */
	virtual bool format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput) = 0;

	virtual bool flush(toolslib::files::IFile *oOutput) = 0;

	/**
	 * Reset the formatter if in case it should be reused.
	 */
	virtual bool init(void) = 0;

	/**
	 * Called when the formatter is done with. init() may be called to reuse it.
	 */
	virtual bool finalize(toolslib::files::IFile *oOutput) = 0;
};

#endif // DUMP_FORMATTER_H
