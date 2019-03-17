#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/CBMFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

CBMFormatter::CBMFormatter(DataFormatter::ByteType type, uint16_t columns, uint16_t startLine, uint16_t stepping, uint16_t startAddress)
: BasicFormatter(type, columns, startLine, stepping, startAddress)
, mCurAddress(startAddress)
{
}

CBMFormatter::CBMFormatter(const BasicFormatter &source)
{
	setType(source.getType());
	setColumns(source.getColumns());
	setStartAddress(source.getStartAddress());
	setStartLine(source.getStartLine());
	setStepping(source.getStepping());

	mCurAddress = getStartAddress();
}

bool CBMFormatter::init(void)
{
	mCurAddress = getStartAddress();

	return super::init();
}

string CBMFormatter::getLinePrefix(void) const
{
	return DataFormatter::getLinePrefix();
}

bool CBMFormatter::writeBuffer(string &buffer, IFile *oOutput, char nNewline)
{
	UNUSED(nNewline);

	string val(buffer);

	buffer = "";

	if (mCurAddress == getStartAddress())
		putVectorValue(buffer, mCurAddress, 16);

	// 2 bytes pointer to next line, 2 bytes for line number, 1 byte DATA, 1 byte end byte 0, + 1 to point to the next line.
	uint64_t n = val.size() + 2 + 2 + 1 + 1 + 1;
	mCurAddress += (uint16_t)n;
	putVectorValue(buffer, mCurAddress, 16);

	n = getStartLine();
	putVectorValue(buffer, n, 16);
	setStartLine(getStartLine() + getStepping());

	n = 0x83; // DATA
	putVectorValue(buffer, n, 8);
	buffer += ' ';
	buffer += val;
	n = 0;
	putVectorValue(buffer, n, 8);

	return DataFormatter::writeBuffer(buffer, oOutput, 0);
}

bool CBMFormatter::finalize(toolslib::files::IFile *oOutput)
{
	uint64_t n = 0;
	string buffer;

	// EOF
	putVectorValue(buffer, n, 16);

	return DataFormatter::writeBuffer(buffer, oOutput, 0);
}
