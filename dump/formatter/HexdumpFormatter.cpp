#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/HexdumpFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

 HexdumpFormatter::HexdumpFormatter(DataFormatter::ByteType type, uint16_t columns, uint16_t nAdressSize, CharMode nCharMode)
: DataFormatter(type, columns, "", "", "", 0)
, mAddress(0)
, mAddressSize(nAdressSize)
, mCharMode(nCharMode)
{
}

bool HexdumpFormatter::init(void)
{
	mBuffer = "";
	mAddress = 0;

	return super::init();
}

std::string HexdumpFormatter::getLinePrefix(void) const
{
	string address;
	address.resize(24);
	address[0] = 0;
	size_t len = 0;

	switch (mAddressSize)
	{
	case 16:
		mAddress &= 0xffff;
		len = sprintf(&address[0], "%04.4X: ", (unsigned int)mAddress);
		break;

	case 32:
		mAddress &= 0xffffffff;
		len = sprintf(&address[0], "%08.8X: ", (unsigned int)mAddress);
		break;

	case 64:
		len = sprintf(&address[0], "%016.16llX: ", mAddress);
		break;
	}
	address.resize(len);

	return address;
}

bool HexdumpFormatter::writeBuffer(IFile *oOutput, char nNewline)
{
	char superNewline = 0;

	if (getCharMode() == NONE)
		superNewline = nNewline;

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

bool HexdumpFormatter::createColumnValue(const char *oData, const char *oEnd, string &oColumnValue)
{
	if (super::createColumnValue(oData, oEnd, oColumnValue) == false)
		return false;

	if (getCharMode() != NONE)
	{
		char c = *oData;
		if (!isprint(c & 0xff))
			c = '.';

		mBuffer += c;
	}

	mAddress++;

	return true;
}
