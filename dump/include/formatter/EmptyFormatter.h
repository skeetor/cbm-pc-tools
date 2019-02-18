#ifndef EMPTY_FORMATTER_H
#define EMPTY_FORMATTER_H

#include "Formatter.h"

/**
 * A formatter which does nothing but passing the data through.
 */
class EmptyFormatter
: public Formatter
{
public:
	EmptyFormatter() {}
	~EmptyFormatter() override {}

	bool format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput, bool bFlush = false) override;
};

#endif // EMPTY_FORMATTER_H
