
#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/EmptyFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

bool EmptyFormatter::format(const char *oData, int64_t nDataSize, IFile *oOutput)
{
	if (oOutput->write(oData, nDataSize) != nDataSize)
		return false;

	return true;
}
