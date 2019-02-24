#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/HexdumpFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

 HexdumpFormatter::HexdumpFormatter(DataFormatter::ByteType type, uint16_t columns, bool bShowAscii, bool bPetsci)
: DataFormatter(type, columns, "", "", "", 0)
, mShowAscii(bShowAscii)
, mPetsci(bPetsci)
{
}

bool HexdumpFormatter::init(void)
{
	mBuffer = "";

	return super::init();
}

bool HexdumpFormatter::writeBuffer(IFile *oOutput, char nNewline)
{
	if (super::writeBuffer(oOutput, 0) == false)
		return false;

	if (!mBuffer.empty())
	{
		uint16_t charcolumns = 1;

		if (getColumnPrefix() != 0)
			charcolumns += 2;			// ", "

		switch (getType())
		{
			case DataFormatter::BIN:
				charcolumns += 9;
			break;

			case DataFormatter::HEX_CBM:
			case DataFormatter::HEX_ASM:
			case DataFormatter::HEX_C:
				charcolumns += 3;
			break;
		}
		uint16_t columns = (getColumns() - (uint16_t)mBuffer.size()) * charcolumns;
		string filler(columns, ' ');

		mBuffer = filler + mBuffer;

		return super::writeBuffer(mBuffer, oOutput, nNewline);
	}

	return true;
}

bool HexdumpFormatter::addToBuffer(const char *oData, const char *oEnd)
{
	if (super::addToBuffer(oData, oEnd) == false)
		return false;

	char c = *oData;
	if (!isprint(c & 0xff))
		c = '.';

	mBuffer += c;

	return true;
}
