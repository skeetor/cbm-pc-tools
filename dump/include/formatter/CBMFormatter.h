#ifndef DUMP_CBMFORMATTER_H
#define DUMP_CBMFORMATTER_H

#include "BasicFormatter.h"

class CBMFormatter
: public BasicFormatter
{
public:
	CBMFormatter(DataFormatter::ByteType type = DEC, uint16_t columns = 16, uint16_t startLine = 1000, uint16_t stepping = 10, uint16_t startAddress = 0x0801);
	CBMFormatter(const BasicFormatter &source);
	~CBMFormatter() override {}

public:
	bool init(void) override;
	bool finalize(toolslib::files::IFile *oOutput) override;

protected:
	bool writeBuffer(std::string &buffer, toolslib::files::IFile *oOutput, char nNewline = '\n') override;
	std::string getLinePrefix(void) const override;

private:
	typedef BasicFormatter super;

private:
	uint16_t mCurAddress;
};

#endif // DUMP_CBMFORMATTER_H
