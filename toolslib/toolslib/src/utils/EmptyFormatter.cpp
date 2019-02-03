
#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "toolslib/utils/EmptyFormatter.h"

using namespace std;

namespace toolslib
{
	namespace utils
	{
		bool EmptyFormatter::format(const string &input, string &output)
		{
			output.reserve(input.size());
			copy(input.begin(), input.end(), output.begin());

			return true;
		}

		bool EmptyFormatter::flush(std::string &output)
		{
			UNUSED(output);

			return true;
		}
	}
}
