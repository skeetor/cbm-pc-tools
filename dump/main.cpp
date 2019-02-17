// dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <string>
#include <iostream>
#include <memory>

#include "toolslib/utils/CommandlineParser.h"
#include "toolslib/files/FileFactory.h"
#include "toolslib/files/IFile.h"
#include "toolslib/files/File.h"

#include "toolslib/strings/numbers.h"

#include "formatter/CA65Formatter.h"
#include "formatter/EmptyFormatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;
using namespace toolslib::files;
using namespace toolslib::strings;
using namespace lib::utils;

size_t cc65ByteDump(string const &filename, size_t start, size_t length)
{
	FILE *f = fopen(filename.c_str(), "rb");

	if (f == nullptr)
	{
		fprintf(stderr, "Error opening file %s\n", filename.c_str());
		return (size_t)-1;
	}

	unsigned char buffer[4096];

	fseek(f, (long)start, SEEK_SET);

	size_t total = 0;

	while (1)
	{
		size_t r = fread(buffer, 1, sizeof(buffer), f);
		if (r == 0)
			break;

		size_t dumped = 0;
		string out;
		out.reserve(4096);

		while (dumped < r && total < length)
		{
			out = ".byte";
			char str[32];
			char comma = ' ';
			for (size_t i = 0; i < 16 && dumped < r && total < length; i++, dumped++, total++)
			{
				sprintf(str, "%c $%02x", comma, buffer[dumped]);
				comma = ',';
				out += str;
			}

			cout << out << endl;
		}
	}

	fclose(f);

	return total;
}

template <typename T>
T fromNumber(const char *number, const char *end, const char **scanned)
{
	// TODO: Trim string

	if (number >= end)
		throw invalid_argument("Number is empty");

	size_t len = end - number;
	if (len == 1)
		return fromDecimal<T>(number, end, scanned);

	char c = number[2];
	if (len == 2 && (c == 'x' || c == 'X'))
		throw invalid_argument("Empty hex string");

	if (*(end - 1) == 'h')
		return fromHex<T>(number, end-1, scanned);

	if(number[0] == '0' && (c == 'x' || c == 'X'))
		return fromHex<T>(&number[2], end, scanned);

	if (number[0] == '$')
		return fromHex<T>(&number[1], end, scanned);

	if(number[0] == '%')
		return fromBinary<T>(&number[1], end, scanned);

	return fromDecimal<T>(number, end, scanned);
}

template <typename T>
T fromNumber(const string& number, const char **scanned)
{
	return fromNumber<T>(&number[0], &number[number.size()], scanned);
}

class FileProcessor
{
public:
	FileProcessor(CommandlineParser &parser, int argc, char *argv[])
	: m_parser(parser)
	, m_result(0)
	, m_startPos(0)
	, m_maxLen(invalid64_t)
	, m_curLen(0)
	, m_formatter(make_unique<EmptyFormatter>())
	, m_input((new File())->setSTDIN(stdin))
	, m_output((new File())->setSTDOUT(stdout))
	{
		createCommandlineOptions(m_parser);

		if (!m_parser.parse(argc, argv))
		{
			uint32_t error = parser.getErrorIndex();
			cerr << "Error with parameter '" << parser.getErrorParam() << "' at position " << to_string(error) << endl;
			m_result = -1;
		}
	}

	IFile *openFile(const vector<string> &oArgs, const IFile::open_mode &oMode)
	{
		Filename fn(oArgs[0]);
		IFile *file = FileFactory::getInstance()->getFile(fn);

		if (file == nullptr)
		{
			cerr << "Uknown filetype:" << fn.getOpenpath() << endl;
			exit(0);
		}

		file->setOpenmode(oMode);
		if (!file->open())
		{
			cerr << "Unable to open file:" << file->getOpenpath() << endl;
			exit(0);
		}

		return file;
	}

	void inputFile(CommandlineParser &oParser, const vector<string> &oArgs)
	{
		UNUSED(oParser);
		IFile::open_mode md = { 0 };

		md.read = true;
		md.binary = true;

		m_input.reset(openFile(oArgs, md));
	}

	void outputFile(CommandlineParser &oParser, const vector<string> &oArgs)
	{
		UNUSED(oParser);

		IFile::open_mode md = { 0 };

		md.write = true;
		md.binary = true;
		md.truncate = true;
		md.create = true;

		m_output.reset(openFile(oArgs, md));
	}

	void formatType(CommandlineParser &oParser, const vector<string> &oArgs)
	{
		UNUSED(oParser);

		cout << __func__ << endl;
	}

	void skipOffset(CommandlineParser &oParser, const vector<string> &oArgs)
	{
		UNUSED(oParser);

		const string &number = oArgs[0];

		int64_t ofs = fromNumber<int64_t>(number, nullptr);
		if (ofs != invalid64_t)
			m_input->seek(ofs, IFile::cur);
	}

	void maxLength(CommandlineParser &oParser, const vector<string> &oArgs)
	{
		UNUSED(oParser);

		cout << __func__ << endl;
	}

	void address(CommandlineParser &oParser, const vector<string> &oArgs)
	{
		UNUSED(oParser);

		cout << __func__ << endl;
	}

	void createCommandlineOptions(CommandlineParser &oParser)
	{
		oParser.setHeader(
			"dump (C) by Gerhard Gruber\n"
			"USAGE:\n"
		);

		oParser.addOption("help", "", "Print help")
			.arguments()
			;

		oParser.addOption("input", "i", "Inputfile")
			.mandatory()
			.arguments()
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { inputFile(oParser, oOption.values().back()); })
			;

		oParser.addOption("output", "o", "Outputfile")
			.mandatory()
			.arguments()
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { outputFile(oParser, oOption.values().back()); })
			;

		oParser.addOption("type", "t", "Output format")
			.arguments()
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { formatType(oParser, oOption.values().back()); })
			;

		oParser.addOption("skip", "s", "Skip first N bytes from inputfile")
			.arguments()
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { skipOffset(oParser, oOption.values().back()); })
			;

		oParser.addOption("length", "l", "Write only N bytes")
			.arguments()
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { maxLength(oParser, oOption.values().back()); })
			;

		oParser.addOption("address", "a", "Write first two bytes with address.")
			.arguments()
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { address(oParser, oOption.values().back()); })
			;
	}

	bool hasHelp(void)
	{
		if (m_parser.hasArgument("help"))
		{
			m_parser.help();
			return true;
		}

		return false;
	}

	int status(void)
	{
		return m_result;
	}

	int run(void)
	{
		if (m_result)
			return m_result;

		char buffer[1024];

		while (!m_input->isEOF())
		{
			int64_t rd = m_input->read(buffer, 1024);
			if (rd <= 0)
			{
				if (rd < 0)
					cerr << "Error reading " << m_input->getOpenpath() << endl;

				break;
			}

			rd = m_output->write(buffer, rd);
			if (rd < 0)
			{
				cerr << "Error writing " << m_input->getOpenpath() << endl;
				break;
			}
		}

		return 0;
	}

private:
	unique_ptr<IFile> m_input;
	unique_ptr<IFile> m_output;
	unique_ptr<Formatter> m_formatter;
	int m_result;
	int64_t m_startPos;
	int64_t m_maxLen;
	int64_t m_curLen;
	CommandlineParser &m_parser;
};

int main(int argc, char* argv[])
{
	CommandlineParser parser;

	FileProcessor processor(parser, argc, argv);

	if (processor.hasHelp())
		return 1;

	int result = processor.status();
	if (result)
		return result;

	result = processor.run();

	//cc65ByteDump(argv[1], 2, -1);

	return result;
}
