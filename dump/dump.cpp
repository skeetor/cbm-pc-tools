// dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <iostream>

#include "toolslib/files/FileFactory.h"
#include "toolslib/files/File.h"

#include "formatter/DataFormatter.h"
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

unique_ptr<IFile> FileProcessor::createFile(const std::string &oFilename)
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

unique_ptr<IFile> FileProcessor::openFile(const vector<string> &oArgs, const IFile::open_mode &oMode)
{
	const string &fn(oArgs[0]);
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

	IFile::open_mode md = { 0 };
	
	md.read = true;
	md.binary = true;

	unique_ptr<IFile> input = openFile(oArgs, md);
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

	m_formatter->finalize(m_output.get());
}

void FileProcessor::outputFile(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	IFile::open_mode md = { 0 };

	md.write = true;
	md.binary = true;
	md.truncate = true;
	md.create = true;

	m_output = openFile(oArgs, md);
}

void FileProcessor::parseData(const vector<string> &oArgs)
{
	m_formatter->finalize(m_output.get());

	DataFormatter *formatter = new DataFormatter();
	m_formatter.reset(formatter);

	size_t i = 1;
	string v = oArgs[i];

	if (v.empty())
	{
		string msg = "Invalid format: " + toString(oArgs);
		throw runtime_error(msg);
	}

	if (isdigit(v[0]))
	{
		uint16_t columns = fromNumber<uint16_t>(v, nullptr);
		formatter->setColumns(columns);
		i++;
		if (i >= oArgs.size())
			return;

		v = oArgs[i];
	}

	if (v == "dec")
		formatter->setType(DataFormatter::DEC);
	else if (v == "bin")
		formatter->setType(DataFormatter::BIN);
	else
	{
		string format;

		size_t pos = v.find('=');
		if (pos != string::npos)
		{
			format = v.substr(0, pos);
			v.erase(0, pos+1);
		}
		else
			format = v;

		// 	data [<columns>] [dec|bin|hex[=cbm(default)|asm|c] [<lineprefix>(default=".byte") <header> <postfix>]
		if (format == "hex")
		{
			if(v.empty() || v == "cbm")
				formatter->setType(DataFormatter::HEX_CBM);
			else if (v == "asm")
				formatter->setType(DataFormatter::HEX_ASM);
			else if (v == "c")
				formatter->setType(DataFormatter::HEX_C);
			else
				formatter->setType(DataFormatter::HEX_CBM);
		}
		else
		{
			string msg = "Invalid format: " + toString(oArgs);
			throw runtime_error(msg);
		}
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
}

void FileProcessor::formatType(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &formatter = oArgs[0];

	if (formatter == "data")
		parseData(oArgs);
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

	m_startPos = fromNumber<int64_t>(number, nullptr);
}

void FileProcessor::maxLength(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &number = oArgs[0];

	m_maxLen = fromNumber<int64_t>(number, nullptr);
}

void FileProcessor::writeData(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &number = oArgs[0];
	if (isdigit(number[0]))
	{
		uint16_t bitness = fromNumber<uint16_t>(number, nullptr);
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
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { inputFile(oParser, oOption.values().back()); })
		;

	oParser.addOption("output", "o", "Outputfile")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { outputFile(oParser, oOption.values().back()); })
		;

	oParser.addOption("type", "t",
R"(Output format type
	data [<columns>] [dec|bin|hex[=cbm(default)|asm|c] [<lineprefix>(default=".byte") <header> <postfix>]
       <columns> = number of columns per line
       cbm = '$a2', asm = '0a2h', c = '0xa2'
       <lineprefix> = user defined string, default is '.byte')
       <header> = printed first (optional)
       <postfix> = added after the last line (optional)
)"
			)
			.arguments(1, 6)
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

	oParser.addOption("write", "w", "Write custom data. First value is 8,16,32,64 to specify the bitness or a filename")
		.multiple()
		.arguments(2, CommandlineParser::UNLIMITED_ARGS)
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

	if (m_result)
		return m_result;

	return 0;
}
