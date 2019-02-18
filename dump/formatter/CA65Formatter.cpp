#define _CRT_SECURE_NO_WARNINGS

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

bool CA65Formatter::format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput, bool bFlush)
{
	const char *end = oData + nDataSize;

	while (oData < end)
	{
		char colon[3] = ", ";

		if (mCurColumn == mColumns)
			mCurColumn = 0;

		if (mCurColumn == 0)
		{
			if (!mBuffer.empty())
			{
				if ((size_t)oOutput->write(&mBuffer[0], mBuffer.size()) != mBuffer.size())
					return false;
			}

			mBuffer = ".byte";
			colon[0] = 0;
		}

		char buffer[8] = { 0 };

		sprintf(buffer, "%s%3u", colon, (unsigned int)(*oData++));

		mBuffer += buffer;
		colon[0] = ',';
		mCurColumn++;
	}

	if (bFlush == true || mCurColumn == mColumns)
	{
		if (mCurColumn == mColumns)
			mCurColumn = 0;

		if (!mBuffer.empty())
		{
			if ((size_t)oOutput->write(&mBuffer[0], mBuffer.size()) != mBuffer.size())
				return false;
		}
	}

	return true;
}
