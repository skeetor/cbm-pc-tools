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
T fromNumber(const std::string& number, const char **scanned)
{
	return fromNumber<T>(&number[0], &number[number.size()], scanned);
}

class FileProcessor
{
public:
	FileProcessor(toolslib::utils::CommandlineParser &parser);
	virtual ~FileProcessor();

	bool hasHelp(void);
	int run(void);
	int status(void);

protected:
	virtual toolslib::files::IFile *createFile(const std::string &oFilename);

	toolslib::files::IFile *openFile(const std::vector<std::string> &oArgs, const toolslib::files::IFile::open_mode &oMode);
	void inputFile(toolslib::utils::CommandlineParser &oParser, const std::vector<std::string> &oArgs);
	void outputFile(toolslib::utils::CommandlineParser &oParser, const std::vector<std::string> &oArgs);
	void formatType(toolslib::utils::CommandlineParser &oParser, const std::vector<std::string> &oArgs);
	void skipOffset(toolslib::utils::CommandlineParser &oParser, const std::vector<std::string> &oArgs);
	void maxLength(toolslib::utils::CommandlineParser &oParser, const std::vector<std::string> &oArgs);
	void address(toolslib::utils::CommandlineParser &oParser, const std::vector<std::string> &oArgs);
	void createCommandlineOptions(toolslib::utils::CommandlineParser &oParser);

	bool parseData(const std::vector<std::string> &oArgs);

private:
	std::unique_ptr<toolslib::files::IFile> m_input;
	std::unique_ptr<toolslib::files::IFile> m_output;
	std::unique_ptr<Formatter> m_formatter;
	int m_result;
	int64_t m_startPos;
	int64_t m_maxLen;
	int64_t m_curLen;
	toolslib::utils::CommandlineParser &m_parser;
};

#endif // DUMP_H
