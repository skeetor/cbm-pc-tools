// dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <iostream>

#include "toolslib/files/FileFactory.h"
#include "toolslib/files/File.h"

#include "formatter/DataFormatter.h"
#include "formatter/HexdumpFormatter.h"
#include "formatter/EmptyFormatter.h"

#include "dump.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;
using namespace toolslib::files;
using namespace toolslib::strings;

namespace
{
	/**
	 * Write data to the specified file from the vector. index specifies the starting index
	 * in the vector.
	 */
	template <typename T>
	void writeVectorData(unique_ptr<IFile> &file, size_t index, const vector<string> &numbers)
	{
		vector<T> values;

		size_t i = index;
		while (i < numbers.size())
		{
			T data = fromNumber<T>(numbers[i], nullptr);
			values.emplace_back(data);
			i++;
		}

		int64_t len = (int64_t)(sizeof(T)*values.size());
		if (file->write(&values[0], len) != len)
		{
			string msg = "Error writing data to file: " + file->getOpenpath();
			throw runtime_error(msg);
		}
	}
}

FileProcessor::FileProcessor(CommandlineParser &parser)
: m_parser(parser)
, m_result(0)
, m_startPos(0)
, m_maxLen(invalid64_t)
, m_formatter(make_unique<EmptyFormatter>())
, m_output((new File())->setSTDOUT(stdout))
{
	createCommandlineOptions(m_parser);
}

FileProcessor::~FileProcessor(void)
{
}

unique_ptr<IFile> FileProcessor::createFile(const string &oFilename)
{
	Filename fn(oFilename);
	return unique_ptr<IFile>(FileFactory::getInstance()->getFile(fn));
}

string FileProcessor::toString(const vector<string> &strings) const
{
	string rc;
	string sep;

	for (const string& s : strings)
	{
		rc += sep + s;
		sep = " ";
	}

	return rc;
}

unique_ptr<IFile> FileProcessor::openFile(const string &oFilename, const IFile::open_mode &oMode)
{
	const string &fn(oFilename);
	unique_ptr<IFile> file = createFile(fn);

	if (file == nullptr)
	{
		string msg = "Unknown filetype: " + fn;
		throw runtime_error(msg);
	}

	file->setOpenmode(oMode);
	if (!file->open())
	{
		string msg = "Unable to open file:" + file->getOpenpath();
		throw runtime_error(msg);
	}

	return file;
}

void FileProcessor::inputFile(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	for (const string &filename : oArgs)
	{
		IFile::open_mode md = { 0 };

		md.read = true;
		md.binary = true;

		unique_ptr<IFile> input = openFile(filename, md);
		char buffer[4096];

		int64_t start = m_startPos;
		int64_t totalLen = m_maxLen;

		m_maxLen = -1;
		m_startPos = invalid64_t;

		if (start != invalid64_t)
			input->seek(start, IFile::cur);

		int64_t bufferLen = sizeof(buffer);
		while (!input->isEOF())
		{
			if (totalLen != invalid64_t && totalLen < bufferLen)
				bufferLen = totalLen;

			int64_t rd = input->read(buffer, bufferLen);
			if (rd <= 0)
			{
				if (rd < 0)
				{
					string msg = "Error reading " + input->getOpenpath();
					throw runtime_error(msg);
				}
				break;
			}

			totalLen -= rd;

			m_formatter->format(buffer, rd, m_output.get());
		}
	}
}

void FileProcessor::outputFile(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	IFile::open_mode md = { 0 };

	md.write = true;
	md.binary = true;
	md.truncate = true;
	md.create = true;

	m_output = openFile(oArgs[0], md);
}

int FileProcessor::parseByteType(string param)
{
	string format;

	size_t pos = param.find('=');
	if (pos != string::npos)
	{
		format = param.substr(0, pos);
		param.erase(0, pos + 1);
	}
	else
	{
		format = param;
		param = "";
	}

	if (format == "dec")
	{
		if (param.empty() || param == "unsigned")
			return DataFormatter::DEC;
		else if (param == "signed")
			return DataFormatter::DEC_SIGNED;
	}
	else if (format == "bin")
		return DataFormatter::BIN;
	else if (format == "hex")
	{
		if (param.empty() || param == "cbm")
			return DataFormatter::HEX_CBM;
		else if (param == "asm")
			return DataFormatter::HEX_ASM;
		else if (param == "c")
			return DataFormatter::HEX_C;
	}

	return DataFormatter::TYPE_INVALID;
}

uint16_t FileProcessor::parseColumn(const string &value, const vector<string> &oArgs)
{
	if (value.empty())
	{
		string msg = "Invalid format: " + toString(oArgs);
		throw runtime_error(msg);
	}

	if (isdigit(value[0]))
		return fromNumber<uint16_t>(value, nullptr, false);

	return (uint16_t)-1;
}

void FileProcessor::parseData(const vector<string> &oArgs)
{
	m_formatter->flush(m_output.get());

	DataFormatter *formatter = new DataFormatter();
	m_formatter.reset(formatter);

	size_t i = 1;
	string v = oArgs[i];

	uint16_t columns;
	if((columns = parseColumn(v, oArgs)) != (uint16_t)-1)
		formatter->setColumns(columns);

	i++;
	if (i >= oArgs.size())
		return;

	v = oArgs[i];

	DataFormatter::ByteType type;
	if ((type = (DataFormatter::ByteType)parseByteType(v)) != DataFormatter::TYPE_INVALID)
		formatter->setType(type);
	else
	{
		string msg = "Invalid format: " + toString(oArgs);
		throw runtime_error(msg);
	}

	i++;
	if (i >= oArgs.size())
		return;

	formatter->setLinePrefix(oArgs[i]);

	i++;
	if (i >= oArgs.size())
		return;

	formatter->setHeader(oArgs[i]);

	i++;
	if (i >= oArgs.size())
		return;

	formatter->setPostfix(oArgs[i]);

	i++;
	if (i >= oArgs.size())
		return;

	char c = 0;
	if (oArgs[i].size() > 0)
		c = oArgs[i][0];

	formatter->setColumnPrefix(c);
}

void FileProcessor::parseHexdump(const vector<string> &oArgs)
{
	m_formatter->flush(m_output.get());

	HexdumpFormatter *formatter = new HexdumpFormatter();
	m_formatter.reset(formatter);

	size_t i = 1;
	string v = oArgs[i];

	uint16_t columns;
	if ((columns = parseColumn(v, oArgs)) != (uint16_t)-1)
		formatter->setColumns(columns);

	i++;
	if (i >= oArgs.size())
		return;

	v = oArgs[i];
}

void FileProcessor::formatType(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &formatter = oArgs[0];

	if (formatter == "data")
		parseData(oArgs);
	else if (formatter == "xd")
		parseHexdump(oArgs);
	else
	{
		string msg = "Unknown type: " + toString(oArgs);
		throw runtime_error(msg);
	}
}

void FileProcessor::skipOffset(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &number = oArgs[0];

	m_startPos = fromNumber<int64_t>(number, nullptr, false);
}

void FileProcessor::maxLength(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &number = oArgs[0];

	m_maxLen = fromNumber<int64_t>(number, nullptr, false);
}

void FileProcessor::writeData(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &number = oArgs[0];
	if (isdigit(number[0]))
	{
		uint16_t bitness = fromNumber<uint16_t>(number, nullptr, false);
		switch (bitness)
		{
			case 8:
				writeVectorData<uint8_t>(m_output, 1, oArgs);
			break;

			case 16:
				writeVectorData<uint16_t>(m_output, 1, oArgs);
			break;

			case 32:
				writeVectorData<uint32_t>(m_output, 1, oArgs);
			break;

			case 64:
				writeVectorData<uint64_t>(m_output, 1, oArgs);
			break;

			default:
			{
				string msg = "Bitness must be 8,16,,32 or 64 : " + number;
				throw runtime_error(msg);
			}
		}
	}
	else
	{
		string msg = "Insert data from file not yet implemented." + number;
		throw runtime_error(msg);
	}
}

void FileProcessor::createCommandlineOptions(CommandlineParser &oParser)
{
	oParser.setHeader(
		"dump 0.1 (C) by Gerhard Gruber\n"
		"USAGE:\n"
	);

	oParser.addOption("help", "", "Print help")
		.arguments(0, 0)
		;

	oParser.addOption("input", "i", "Inputfile")
		.multiple()
		.mandatory()
		.arguments(1, CommandlineParser::UNLIMITED_ARGS)
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { inputFile(oParser, oOption.values().back()); })
		;

	oParser.addOption("output", "o", "Outputfile")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { outputFile(oParser, oOption.values().back()); })
		;

	oParser.addOption("type", "t",
R"(Output format type
    data [<columns>] [dec[=unsigned(default)|signed]|bin|hex[=cbm(default)|asm|c] [<lineprefix>(default=".byte") <header> <postfix> <column separator>]
       <columns> = number of columns per line
       cbm = '$a2', asm = '0a2h', c = '0xa2'
       <lineprefix> = user defined string, default is '.byte')
       <header> = printed first (optional)
       <postfix> = added after the last line (optional)
       <column separator> = added after the last line (optional)
    xd [columns] [dec[=unsigned(default)|signed]|bin|hex[=cbm(default)|asm|c] [ascii=default|screen|petsci|off]
)"
			)
			.arguments(1, 7)
			.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { formatType(oParser, oOption.values().back()); })
		;

	oParser.addOption("skip", "s", "Skip first N bytes from inputfile")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { skipOffset(oParser, oOption.values().back()); })
		;

	oParser.addOption("length", "l", "Write only N bytes")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { maxLength(oParser, oOption.values().back()); })
		;

	oParser.addOption("write", "w", "Write custom data. First value may be 8,16,32,64 to specify the bitness or a filename")
		.multiple()
		.arguments(1, CommandlineParser::UNLIMITED_ARGS)
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { writeData(oParser, oOption.values().back()); })
		;
}

bool FileProcessor::hasHelp(void)
{
	if (m_parser.hasArgument("help"))
	{
		m_parser.help();
		return true;
	}

	return false;
}

int FileProcessor::status(void)
{
	return m_result;
}

int FileProcessor::run(void)
{
	if (!m_parser.parse())
	{
		m_result = -1;
		uint32_t error = m_parser.getErrorIndex();
		string msg = "Error with parameter '" + m_parser.getErrorParam() + "' at position " + to_string(error);
		throw runtime_error(msg);
	}

	m_formatter->finalize(m_output.get());
	m_output->close();

	if (m_result)
		return m_result;

	return 0;
}
