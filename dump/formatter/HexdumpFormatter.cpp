#define _CRT_SECURE_NO_WARNINGS

#include <algorithm>

#include "toolslib/toolslib_def.h"
#include "formatter/HexdumpFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;


namespace
{
}

 HexdumpFormatter::HexdumpFormatter(DataFormatter::ByteType type, uint16_t columns, bool bShowAscii, bool bPetsci)
: DataFormatter(type, columns, "", "", "", 0)
, mShowAscii(bShowAscii)
, mPetsci(bPetsci)
{
}

bool HexdumpFormatter::format(const char *oData, int64_t nDataSize, IFile *oOutput)
{
	if (super::format(oData, nDataSize, oOutput) == false)
		return false;

	return true;
}

bool HexdumpFormatter::init(void)
{
	return super::init();
}

bool HexdumpFormatter::finalize(IFile *oOutput)
{
	return super::finalize(oOutput);
}

bool HexdumpFormatter::flush(IFile *oOutput)
{
	return super::flush(oOutput);
}
