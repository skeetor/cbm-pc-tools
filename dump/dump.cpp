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

FileProcessor::FileProcessor(CommandlineParser &parser)
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

	if (!m_parser.parse())
	{
		uint32_t error = parser.getErrorIndex();
		cerr << "Error with parameter '" << parser.getErrorParam() << "' at position " << to_string(error) << endl;
		m_result = -1;
	}
}

FileProcessor::~FileProcessor(void)
{
}

unique_ptr<IFile> FileProcessor::createFile(const std::string &oFilename)
{
	Filename fn(oFilename);
	return unique_ptr<IFile>(FileFactory::getInstance()->getFile(fn));
}

unique_ptr<IFile> FileProcessor::openFile(const vector<string> &oArgs, const IFile::open_mode &oMode)
{
	const string &fn(oArgs[0]);
	unique_ptr<IFile> file = createFile(fn);

	if (file == nullptr)
	{
		cerr << "Uknown filetype:" << fn << endl;
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

void FileProcessor::inputFile(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);
	IFile::open_mode md = { 0 };

	md.read = true;
	md.binary = true;

	m_input = openFile(oArgs, md);
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

bool FileProcessor::parseData(const std::vector<std::string> &oArgs)
{
	return false;
}

void FileProcessor::formatType(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &formatter = oArgs[0];

	if (formatter == "data")
	{
		if (!parseData(oArgs))
			exit(1);
	}
}

void FileProcessor::skipOffset(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	const string &number = oArgs[0];

	int64_t ofs = fromNumber<int64_t>(number, nullptr);
	if (ofs != invalid64_t)
		m_input->seek(ofs, IFile::cur);
}

void FileProcessor::maxLength(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	cout << __func__ << endl;
}

void FileProcessor::address(CommandlineParser &oParser, const vector<string> &oArgs)
{
	UNUSED(oParser);

	cout << __func__ << endl;
}

void FileProcessor::createCommandlineOptions(CommandlineParser &oParser)
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

	oParser.addOption("type", "t",
R"(Output format type
	data [<columns>] [dec|bin|hex [cbm(default)|asm|c] [<lineprefix>(default=".byte") <header> <postfix>]
       <columns> = number of columns per line
       cbm = '$a2', asm = '0a2h', c = '0xa2'
       <lineprefix> = user defined string, default is '.byte')
       <header> = printed first (optional)
       <postfix> = added after the last line (optional)
)"
			)
			.arguments(1, 5)
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
