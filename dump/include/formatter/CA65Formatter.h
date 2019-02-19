#ifndef DUMP_CA65FORMATTER_H
#define DUMP_CA65FORMATTER_H

#include "Formatter.h"

class CA65Formatter
: public Formatter
{
public:
	typedef enum
	{
		HEX,
		DEC,
		BIN
	} ByteType;

public:
	CA65Formatter(ByteType type = HEX, uint16_t columns = 16);
	~CA65Formatter() override {}

	bool format(const char *oData, int64_t nDataSize, toolslib::files::IFile *oOutput) override;
	bool flush(toolslib::files::IFile *oOutput) override;

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

protected:
	bool writeBuffer(toolslib::files::IFile *oOutput);

private:
	ByteType mType;
	uint16_t mColumns;
	uint16_t mCurColumn;
	std::string mBuffer;
};

#endif // DUMP_CA65FORMATTER_H
