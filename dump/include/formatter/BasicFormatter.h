#ifndef DUMP_BASICFORMATTER_H
#define DUMP_BASICFORMATTER_H

#include "DataFormatter.h"

class BasicFormatter
: public DataFormatter
{
public:
public:
	BasicFormatter(DataFormatter::ByteType type = DEC, uint16_t columns = 16, uint16_t startLine = 1000, uint16_t stepping = 10);
	~BasicFormatter() override {}

	bool init(void) override;

public:
	void setStartLine(uint16_t nStartLine)
	{
		mStartLine = nStartLine;
	}

	uint16_t getStartLine(void) const
	{
		return mStartLine;
	}

	void setStepping(uint16_t nStepping)
	{
		mStepping = nStepping;
	}

	uint16_t getStepping(void) const
	{
		return mStepping;
	}

	std::string getLinePrefix(void) const override;

private:
	typedef DataFormatter super;

private:
	uint16_t mStartLine;
	mutable uint16_t mCurLine;
	uint16_t mStepping;
};

#endif // DUMP_BASICFORMATTER_H
