
#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/CA65Formatter.h"

using namespace std;

CA65Formatter::CA65Formatter(ByteType type, uint16_t columns)
: mType(type)
, mColumns(columns)
, mCurColumn(0)
{
}

bool CA65Formatter::format(const string &input, string &output)
{
	output.reserve(input.size());
	copy(input.begin(), input.end(), output.begin());

	return true;
}

bool CA65Formatter::flush(string &output)
{
	UNUSED(output);

	return true;
}

void CA65Formatter::reset(void)
{
}
