#ifndef EMPTY_FORMATTER_H
#define EMPTY_FORMATTER_H

#include "Formatter.h"

/**
 * A formatter which does nothing but passing the data through.
 */
namespace toolslib
{
	namespace utils
	{
		class EmptyFormatter
		: public Formatter
		{
		public:
			EmptyFormatter() {}
			~EmptyFormatter() override {}

			bool format(const std::string &input, std::string &output) override;

			bool flush(std::string &output) override;
		};
	}
}

#endif // EMPTY_FORMATTER_H
