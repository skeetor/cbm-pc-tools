#ifndef DUMP_HEXDUMPFORMATTER_H
#define DUMP_HEXDUMPFORMATTER_H

#include "DataFormatter.h"

class HexdumpFormatter
: public DataFormatter
{
public:
	typedef enum
	{
		NONE,
		ASCII,
		PETSCI,
		CBM_SCREEN,			// C64, VC20, etc.

		MODE_INVALID
	} CharMode;

public:
	HexdumpFormatter(DataFormatter::ByteType type = HEX, uint16_t columns = 16, uint16_t nAdressSize = 16, CharMode nCharMode = ASCII);
	~HexdumpFormatter() override {}

	bool init(void) override;

public:
	/**
	 * Sets the input format. i.E. if the mode is CBM_SCREEN and the char is 0x01 an 'A' is displayed.
	 * This works only partially, because not all CBM screen characters may be displayed as a character
	 * here.
	 */
	void setCharMode(CharMode nCharMode)
	{
		mCharMode = nCharMode;
	}

	CharMode getCharMode(void) const
	{
		return mCharMode;
	}

	void setAddressSize(uint16_t nAddressSize)
	{
		mAddressSize = nAddressSize;
	}

	uint16_t getAddressSize(void) const
	{
		return mAddressSize;
	}

	std::string getLinePrefix(void) const override;

protected:
	bool writeBuffer(toolslib::files::IFile *oOutput, char nNewline = '\n') override;
	bool createColumnValue(const char *oData, const char *oEnd, std::string &oColumnValue) override;

private:
	typedef DataFormatter super;

private:
	std::string mBuffer;
	mutable uint64_t mAddress;
	uint16_t mAddressSize;
	CharMode mCharMode;
};

#endif // DUMP_HEXDUMPFORMATTER_H
