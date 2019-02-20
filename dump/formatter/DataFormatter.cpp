#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/DataFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

DataFormatter::DataFormatter(ByteType type, uint16_t columns, const std::string &linePrefix, const std::string &header, const std::string &postfix)
: mType(type)
, mColumns(columns)
, mCurColumn(0)
, mLinePrefix(linePrefix)
, mHeader(header)
, mPostfix(postfix)
, mAddHeader(true)
{
}

bool DataFormatter::writeBuffer(toolslib::files::IFile *oOutput)
{
	mCurColumn = 0;

	bool rc = true;

	if (!mBuffer.empty())
	{
		mBuffer += '\n';
		if ((size_t)oOutput->write(&mBuffer[0], mBuffer.size()) != mBuffer.size())
			rc = false;
	}

	mBuffer = "";

	return true;
}

bool DataFormatter::format(const char *oData, int64_t nDataSize, IFile *oOutput)
{
	const char *end = oData + nDataSize;

	while (oData < end)
	{
		char colon[3] = ", ";

		if (mCurColumn == mColumns)
			mCurColumn = 0;

		if (mCurColumn == 0)
		{
			if (mAddHeader)
			{
				mAddHeader = false;
				mBuffer = mHeader;
			}

			if (!writeBuffer(oOutput))
				return false;

			mBuffer = ".byte ";
			colon[0] = 0;
		}

		char buffer[12] = { 0 };

		if (mType == DEC)
			sprintf(buffer, "%s%u", colon, (unsigned int)(*oData++) & 0xff);
		else if (mType == HEX_CBM)
			sprintf(buffer, "%s$%02x", colon, (unsigned int)(*oData++) & 0xff);
		else if (mType == HEX_ASM)
		{
			char value[4] = { 0 };
			unsigned int c = *((unsigned char *)oData);
			oData++;
			sprintf(value, "%xh", c & 0xff);
			if (isalpha(value[0]))
				sprintf(buffer, "%s0%s", colon, value);
			else
				strcpy(buffer, value);
		}
		else if (mType == HEX_C)
			sprintf(buffer, "%s0x%02x", colon, (unsigned int)(*oData++) & 0xff);
		else if (mType == BIN)
		{
			sprintf(buffer, "%s%02x", colon, (unsigned int)(*(oData++)) & 0xff);
		}
		else
			return false;

		mBuffer += buffer;
		colon[0] = ',';
		mCurColumn++;
	}

	if (mCurColumn == mColumns)
	{
		if (mCurColumn == mColumns)
			mCurColumn = 0;

		return writeBuffer(oOutput);
	}

	return true;
}
bool DataFormatter::init(void)
{
	mCurColumn = 0;
	mAddHeader = true;

	return true;
}

bool DataFormatter::finalize(toolslib::files::IFile *oOutput)
{
	mBuffer = mPostfix;
	return writeBuffer(oOutput);
}

bool DataFormatter::flush(IFile *oOutput)
{
	return writeBuffer(oOutput);
}
