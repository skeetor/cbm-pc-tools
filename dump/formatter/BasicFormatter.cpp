#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/BasicFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

 BasicFormatter::BasicFormatter(DataFormatter::ByteType type, uint16_t columns, uint16_t startLine, uint16_t stepping)
: DataFormatter(type, columns, "", "", "", 0)
, mStartLine(startLine)
, mCurLine(startLine)
, mStepping(stepping)
{
}

bool BasicFormatter::init(void)
{
	mBuffer = "";
	mCurLine = mStartLine;

	return super::init();
}

std::string BasicFormatter::getLinePrefix(void) const
{
	return to_string(mCurLine);
}

bool BasicFormatter::writeBuffer(IFile *oOutput, char nNewline)
{
	char superNewline = 0;

	if (super::writeBuffer(oOutput, superNewline) == false)
		return false;

	if (mBuffer.empty())
		return true;

	// Space is always a column separator
	uint16_t charcolumns = 1;

	switch (getType())
	{
		case DataFormatter::HEX_ASM:
		case DataFormatter::HEX_C:
		case DataFormatter::DEC_SIGNED:
			charcolumns += 4;
		break;
	
		case DataFormatter::BIN:
			charcolumns += 9;
		break;

		case DataFormatter::DEC:
		case DataFormatter::HEX_CBM:
			charcolumns += 3;
		break;

		case DataFormatter::HEX:
			charcolumns += 2;
		break;
	}

	uint16_t columns = (getColumns() - (uint16_t)mBuffer.size()) * charcolumns;
	string filler(columns+2, ' ');
	filler += mBuffer;
	mBuffer = "";

	return super::writeBuffer(filler, oOutput, nNewline);
}
