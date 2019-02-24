#ifndef DUMP_DATAFORMATTER_H
#define DUMP_DATAFORMATTER_H

#include "Formatter.h"

class DataFormatter
: public Formatter
{
public:
	typedef enum
	{
		DEC,
		DEC_SIGNED,
		BIN,
		HEX_CBM,			// $ae
		HEX_ASM,			// 0aeh, 12h
		HEX_C,				// 0xae

	} ByteType;

public:
	DataFormatter(ByteType type = HEX_CBM, uint16_t columns = 16, const std::string &lineprefix = ".byte ", const std::string &header = "", const std::string &postfix = "");
	~DataFormatter() override {}

	bool format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput) override;
	bool flush(toolslib::files::IFile *oOutput) override;
	bool init(void) override;
	bool finalize(toolslib::files::IFile *oOutput) override;

public:
	ByteType getType(void) const
	{
		return mType;
	}

	void setType(ByteType type)
	{
		mType = type;
	}

	uint16_t getColumns(void) const
	{
		return mColumns;
	}

	void setColumns(uint16_t columns)
	{
		if (columns == 0)
			columns = 1;

		mColumns = columns;
	}

	void setHeader(const std::string &oHeader)
	{
		mHeader = oHeader;
	}

	std::string getHeader(void) const
	{
		return mHeader;
	}

	void setLinePrefix(const std::string &oLinePrefix)
	{
		mLinePrefix = oLinePrefix;
	}

	std::string getLinePrefix(void) const
	{
		return mLinePrefix;
	}

	void setPostfix(const std::string &oPostfix)
	{
		mPostfix = oPostfix;
	}

	std::string getPostfix(void) const
	{
		return mPostfix;
	}

protected:
	bool writeBuffer(toolslib::files::IFile *oOutput);

private:
	ByteType mType;
	uint16_t mColumns;
	uint16_t mCurColumn;
	std::string mBuffer;
	std::string mHeader;
	std::string mLinePrefix;
	std::string mPostfix;
	bool mAddHeader;
};

#endif // DUMP_DATAFORMATTER_H
