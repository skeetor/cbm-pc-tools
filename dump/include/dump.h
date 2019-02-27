#ifndef DUMP_H
#define DUMP_H

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <vector>
#include <memory>

#include "toolslib/utils/CommandlineParser.h"
#include "toolslib/files/IFile.h"
#include "toolslib/strings/numbers.h"

class Formatter;

template <typename T>
T fromNumber(const char *number, const char *end, const char **scanned)
{
	// TODO: Trim std::string

	if (number >= end)
		throw invalid_argument("Number is empty");

	size_t len = end - number;
	if (len == 1)
		return toolslib::strings::fromDecimal<T>(number, end, scanned);

	char c = number[2];
	if (len == 2 && (c == 'x' || c == 'X'))
		throw invalid_argument("Empty hex std::string");

	if (*(end - 1) == 'h')
		return toolslib::strings::fromHex<T>(number, end-1, scanned);

	if(number[0] == '0' && (c == 'x' || c == 'X'))
		return toolslib::strings::fromHex<T>(&number[2], end, scanned);

	if (number[0] == '$')
		return toolslib::strings::fromHex<T>(&number[1], end, scanned);

	if(number[0] == '%')
		return toolslib::strings::fromBinary<T>(&number[1], end, scanned);

	return toolslib::strings::fromDecimal<T>(number, end, scanned);
}

template <typename T>
T fromNumber(const std::string& number, const char **scanned, bool bAllowChar = true)
{
	if (bAllowChar && (number.size() == 3 && number[0] == '\'' && number[2] == '\''))
		return (T)number[1];

	return fromNumber<T>(&number[0], &number[number.size()], scanned);
}

class FileProcessor
{
public:
	FileProcessor(toolslib::utils::CommandlineParser &parser);
	virtual ~FileProcessor();

	int run(void);
	int status(void);

	std::string toString(const std::vector<std::string> &strings) const;

protected:
	virtual std::unique_ptr<toolslib::files::IFile> createFile(const std::string &oFilename);

	std::unique_ptr<toolslib::files::IFile> openFile(const std::string &oArgs, const toolslib::files::IFile::open_mode &oMode);

	void createCommandlineOptions(toolslib::utils::CommandlineParser &oParser);
	int parseByteType(std::string format);
	uint16_t parseColumn(const std::string &value, const std::vector<std::string> &oArgs);

	// Option callbacks
	void inputFile(const std::vector<std::string> &oArgs);
	void outputFile(const std::vector<std::string> &oArgs);
	void skipOffset(const std::vector<std::string> &oArgs);
	void maxLength(const std::vector<std::string> &oArgs);
	void writeData(const std::vector<std::string> &oArgs);
	void dumpData(const std::vector<std::string> &oArgs);
	void dumpHexdump(const std::vector<std::string> &oArgs);

private:
	std::unique_ptr<toolslib::files::IFile> m_output;
	std::unique_ptr<Formatter> m_formatter;
	int m_result;
	int64_t m_startPos;
	int64_t m_maxLen;
	toolslib::utils::CommandlineParser &m_parser;
};

#endif // DUMP_H
