#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/DataFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;


namespace
{
	void toBinary(char *str, const char *end, uint8_t value)
	{
		for (int i = 0; i < 8 && str < end; i++)
		{
			char c = (value >> 7 & 1) + '0';
			value <<= 1;
			*str++ = c;
		}

		if (str < end)
			*str = 0;
	}
}

DataFormatter::DataFormatter(ByteType type, uint16_t columns, const std::string &linePrefix, const std::string &header, const std::string &postfix, char columnPrefix)
: mType(type)
, mColumns(columns)
, mCurColumn(0)
, mLinePrefix(linePrefix)
, mHeader(header)
, mPostfix(postfix)
, mAddHeader(true)
, mColumnPrefix(columnPrefix)
{
}

bool DataFormatter::writeBuffer(IFile *oOutput)
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
		colon[0] = getColumnPrefix();
		if (colon[0] == 0)
		{
			colon[0] = ' ';
			colon[1] = 0;
		}

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

			mBuffer = getLinePrefix();
			colon[0] = 0;
		}

		char buffer[12] = { 0 };
		char c = *oData;
		unsigned char uc = *oData++;

		if (mType == DEC)
			sprintf(buffer, "%s%u", colon, (unsigned int)(uc) & 0xff);
		else if (mType == DEC_SIGNED)
			sprintf(buffer, "%s%d", colon, c);
		else if (mType == HEX_CBM)
			sprintf(buffer, "%s$%02x", colon, (unsigned int)(uc) & 0xff);
		else if (mType == HEX_ASM)
		{
			char value[4] = { 0 };
			sprintf(value, "%02xh", (unsigned int)(uc) & 0xff);
			if (isalpha(value[0]))
				sprintf(buffer, "%s0%s", colon, value);
			else
				sprintf(buffer, "%s%s", colon, value);
		}
		else if (mType == HEX_C)
			sprintf(buffer, "%s0x%02x", colon, (unsigned int)(uc) & 0xff);
		else if (mType == BIN)
		{
			buffer[0] = '%';
			toBinary(&buffer[1], buffer + sizeof(buffer) - 2, uc);
		}
		else
			return false;

		mBuffer += buffer;
		colon[0] = getColumnPrefix();
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

bool DataFormatter::finalize(IFile *oOutput)
{
	mBuffer += mPostfix;
	return writeBuffer(oOutput);
}

bool DataFormatter::flush(IFile *oOutput)
{
	return writeBuffer(oOutput);
}
