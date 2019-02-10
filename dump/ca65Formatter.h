#ifndef DUMP_CA65FORMATTER_H
#define DUMP_CA65FORMATTER_H

#include "lib/utils/Formatter.h"

class CA65Formatter
: public lib::utils::Formatter
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

	bool format(const std::string &input, std::string &output) override;
	bool flush(std::string &output) override;
	void reset(void) override;

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
		mColumns = columns;
	}

private:
	ByteType mType;
	uint16_t mColumns;
	uint16_t mCurColumn;
};

#endif // DUMP_CA65FORMATTER_H
