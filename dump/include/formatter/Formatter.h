#ifndef TOOLSLIB_FORMATTER_H
#define TOOLSLIB_FORMATTER_H

#include <string>

#include "toolslib/toolslib_api.h"

namespace lib
{
	namespace utils
	{
		class TOOLSLIB_API Formatter
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

			/**
			  * Resets the formatter to it's original state.
			  */
			virtual void reset(void) = 0;
		};
	}
}

#endif // TOOLSLIB_FORMATTER_H
