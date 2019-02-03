#ifndef DUMP_FORMATTER_H
#define DUMP_FORMATTER_H

#include <string>

class Formatter
{
public:
	virtual ~Formatter() {}

	/**
	 * Formats the input data into the output data.
	 */
	virtual bool format(const std::string &input, std::string &output) = 0;

	/**
	 * Allows to output a possible reamainder of formatted data.
	 */
	virtual bool flush(std::string &output) = 0;
};

#endif // DUMP_FORMATTER_H
