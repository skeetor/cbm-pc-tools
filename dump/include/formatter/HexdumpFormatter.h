#ifndef DUMP_HEXDUMPFORMATTER_H
#define DUMP_HEXDUMPFORMATTER_H

#include "DataFormatter.h"

class HexdumpFormatter
: public DataFormatter
{
public:
	HexdumpFormatter(DataFormatter::ByteType type = HEX_CBM, uint16_t columns = 16, bool bShowAscii = true, bool bPetsci = false);
	~HexdumpFormatter() override {}

	bool format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput) override;
	bool flush(toolslib::files::IFile *oOutput) override;
	bool init(void) override;
	bool finalize(toolslib::files::IFile *oOutput) override;

public:
	void setShowAscii(bool bShowAscii)
	{
		mShowAscii = bShowAscii;
	}

	bool isShowAscii(void) const
	{
		return mShowAscii;
	}

	void setPetsci(bool bPetsci)
	{
		mPetsci = bPetsci;
	}

	bool isPetsci(void) const
	{
		return mPetsci;
	}

private:
	typedef DataFormatter super;

private:
	bool mShowAscii : 1;
	bool mPetsci : 1;		// PETSCI format if true, otherwise ASCII
};

#endif // DUMP_HEXDUMPFORMATTER_H
