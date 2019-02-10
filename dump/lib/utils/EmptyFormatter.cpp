
#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "lib/utils/EmptyFormatter.h"

using namespace std;

namespace lib
{
	namespace utils
	{
		bool EmptyFormatter::format(const string &input, string &output)
		{
			output.reserve(input.size());
			copy(input.begin(), input.end(), output.begin());

			return true;
		}

		bool EmptyFormatter::flush(string &output)
		{
			UNUSED(output);

			return true;
		}

		void reset(void)
		{
		}
	}
}
