#ifndef DUMP_FORMATTER_H
#define DUMP_FORMATTER_H

#include <string>

#include "toolslib/toolslib_api.h"
#include "toolslib/files/IFile.h"

template <typename T>
void putVectorValue(T &values, uint64_t value, uint16_t size)
{
	uint8_t buffer[sizeof(uint64_t)];

	switch (size)
	{
	case 8:
		size = 1;
		*(uint8_t *)(&buffer[0]) = (uint8_t)(value & 0xff);
		break;

	case 16:
		size = 2;
		*(uint16_t *)(&buffer[0]) = (uint16_t)(value & 0xffff);
		break;

	case 32:
		size = 4;
		*(uint32_t *)(&buffer[0]) = (uint32_t)(value & 0xffffffff);
		break;

	case 64:
		size = 8;
		*(uint64_t *)(&buffer[0]) = value;
		break;

	default:
		string msg = "Invalid data size spezified (8,16,32,64): " + to_string(size);
		throw runtime_error(msg);
	}

	for (uint16_t i = 0; i < size; i++)
		values.push_back(buffer[i]);
}

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
