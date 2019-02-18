
#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/EmptyFormatter.h"

using namespace std;

bool EmptyFormatter::format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput, bool bFlush)
{
	if (oOutput->write(oData, nDataSize) != nDataSize)
		return false;

	return true;
}
