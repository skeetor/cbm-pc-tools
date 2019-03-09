#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/BasicFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

 BasicFormatter::BasicFormatter(DataFormatter::ByteType type, uint16_t columns, uint16_t startLine, uint16_t stepping)
: DataFormatter(type, columns, "", ',')
, mStartLine(startLine)
, mCurLine(startLine)
, mStepping(stepping)
{
}

std::string BasicFormatter::getLinePrefix(void) const
{
	string line = to_string(mCurLine) + " DATA ";
	mCurLine += mStepping;

	return line;
}
