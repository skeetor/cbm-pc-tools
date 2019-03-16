// dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <iostream>

#include "toolslib/files/FileFactory.h"
#include "toolslib/files/File.h"

#include "formatter/DataFormatter.h"
#include "formatter/HexdumpFormatter.h"
#include "formatter/BasicFormatter.h"
#include "formatter/CBMFormatter.h"
#include "formatter/EmptyFormatter.h"

#include "dump.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;
using namespace toolslib::files;
using namespace toolslib::strings;

namespace
{
	void parseParam(string arg, function<void(const string &param, const string &arg)> handler)
	{
		string param;
		size_t pos = arg.find('=');
		if (pos != string::npos)
		{
			param = arg.substr(0, pos);
			arg.erase(0, pos + 1);
		}
		else
		{
			param = arg;
			arg = "";
		}

		handler(param, arg);
	}

	class Initalizer
	{
	public:
		Initalizer(unique_ptr<Formatter> *formatter)
		: m_formatter(formatter)
		{
		}

		~Initalizer()
		{
			(*m_formatter)->init();
		}

	private:
		unique_ptr<Formatter> *m_formatter;
	};
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

void FileProcessor::inputFile(const vector<string> &oArgs)
{
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

		m_formatter->finalize(m_output.get());
	}
}

void FileProcessor::outputFile(const vector<string> &oArgs)
{
	IFile::open_mode md = { 0 };

	md.write = true;
	md.binary = true;
	md.truncate = true;
	md.create = true;

	m_output = openFile(oArgs[0], md);
}

int FileProcessor::parseByteType(string param, bool bCbmDefault)
{
	string format;
	parseParam(param,
		[&](const string &par, const string &arg)
		{
			format = par;
			param = arg;
		}
	);

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
		if (param == "cbm")
			return DataFormatter::HEX_CBM;
		else if (param == "asm")
			return DataFormatter::HEX_ASM;
		else if (param == "c")
			return DataFormatter::HEX_C;
		else
		{
			if(bCbmDefault)
				return DataFormatter::HEX_CBM;
			else
				return DataFormatter::HEX;
		}
	}

	return DataFormatter::TYPE_INVALID;
}

uint64_t FileProcessor::parseNumber(const string &value, const vector<string> &oArgs, bool &bValid)
{
	if (value.empty())
	{
		string msg = "Invalid format: " + toString(oArgs);
		throw runtime_error(msg);
	}

	if (isNumber(value))
	{
		bValid = true;
		return fromNumber<uint64_t>(value, nullptr);
	}

	bValid = false;
	return (uint64_t)-1;
}

bool FileProcessor::isNumber(const std::string &value) const
{
	if (value.empty())
		return false;

	char c = value[0];

	if (isdigit(c))
		return true;

	if (c == '$' || c == '%')
		return true;

	return false;
}

void FileProcessor::dumpData(const vector<string> &oArgs)
{
	m_formatter->flush(m_output.get());

	DataFormatter *formatter = new DataFormatter();
	m_formatter.reset(formatter);
	Initalizer init(&m_formatter);

	if (oArgs.empty())
		return;

	size_t i = 0;
	string v = oArgs[i];
	bool valid = false;
	uint16_t columns = 0;

	if(!v.empty() && (columns = (uint16_t)parseNumber(v, oArgs, valid)) != (uint16_t)-1 || valid == true)
	{
		formatter->setColumns(columns);

		i++;
		if (i >= oArgs.size())
			return;
		v = oArgs[i];
	}

	DataFormatter::ByteType type;
	if ((type = (DataFormatter::ByteType)parseByteType(v)) != DataFormatter::TYPE_INVALID)
	{
		formatter->setType(type);

		i++;
		if (i >= oArgs.size())
			return;
		v = oArgs[i];
	}

	formatter->setLinePrefix(oArgs[i]);

	i++;
	if (i >= oArgs.size())
		return;

	char c = 0;
	if (oArgs[i].size() > 0)
		c = oArgs[i][0];

	formatter->setColumnPrefix(c);
}

void FileProcessor::dumpHexdump(const vector<string> &oArgs)
{
	m_formatter->flush(m_output.get());

	HexdumpFormatter *formatter = new HexdumpFormatter();
	m_formatter.reset(formatter);
	Initalizer init(&m_formatter);

	if (oArgs.empty())
		return;

	size_t i = 0;
	string v = oArgs[i];
	uint16_t val;
	DataFormatter::ByteType type;
	bool valid;

	if ((val = (uint16_t)parseNumber(v, oArgs, valid)) != (uint16_t)-1 || valid == true)
	{
		formatter->setColumns(val);

		i++;
		if (i >= oArgs.size())
			return;

		v = oArgs[i];
	}

	if ((type = (DataFormatter::ByteType)parseByteType(v, false)) != DataFormatter::TYPE_INVALID)
	{
		formatter->setType(type);

		i++;
		if (i >= oArgs.size())
			return;

		v = oArgs[i];
	}

	parseParam(v,
		[&](const string &format, const string &arg)
		{
			if (format == "ascii")
			{
				if (arg == "off")
					formatter->setCharMode(HexdumpFormatter::NONE);
				else
				{
					string msg = "Not yet implemented: " + arg;
					throw runtime_error(msg);
				}

				i++;
				if (i >= oArgs.size())
					return;

				v = oArgs[i];
			}
		}
	);

	if ((val = (uint16_t)parseNumber(v, oArgs, valid)) != (uint16_t)-1 || valid == true)
		formatter->setAddressSize(val);
}

void FileProcessor::dumpBasic(const vector<string> &oArgs)
{
	m_formatter->flush(m_output.get());

	BasicFormatter *formatter = new BasicFormatter();
	m_formatter.reset(formatter);
	Initalizer init(&m_formatter);

	if (oArgs.empty())
		return;

	size_t i = 0;
	string v = oArgs[i];
	uint16_t val;
	DataFormatter::ByteType type;
	bool valid;

	if ((val = (uint16_t)parseNumber(v, oArgs, valid)) != (uint16_t)-1 || valid == true)
	{
		formatter->setColumns(val);

		i++;
		if (i >= oArgs.size())
			return;

		v = oArgs[i];
	}

	if ((type = (DataFormatter::ByteType)parseByteType(v, false)) != DataFormatter::TYPE_INVALID)
	{
		formatter->setType(type);

		i++;
		if (i >= oArgs.size())
			return;
	}

	for (; i < oArgs.size(); i++)
	{
		v = oArgs[i];

		parseParam(v,
			[&](const string &param, const string &arg)
			{
				if (param == "type")
				{
					if (arg.empty() || arg == "ansi")
					{
						// Nothing to do...
					}
					else if (arg == "cbm")
					{
						formatter = new CBMFormatter(*formatter);
						m_formatter.reset(formatter);
					}
					else
					{
						string msg = "Invalid format: " + toString(oArgs);
						throw runtime_error(msg);
					}
				}
				else if (param == "linennumber")
				{
					if ((val = (uint16_t)parseNumber(arg, oArgs, valid)) != (uint16_t)-1 || valid == true)
						formatter->setStartLine(val);
					else
					{
						string msg = "Invalid format: " + toString(oArgs);
						throw runtime_error(msg);
					}
				}
				else if (param == "stepping")
				{
					if ((val = (uint16_t)parseNumber(arg, oArgs, valid)) != (uint16_t)-1 || valid == true)
						formatter->setStepping(val);
					else
					{
						string msg = "Invalid format: " + toString(oArgs);
						throw runtime_error(msg);
					}
				}
				else if (param == "address")
				{
					if ((val = (uint16_t)parseNumber(arg, oArgs, valid)) != (uint16_t)-1 || valid == true)
						formatter->setStartAddress(val);
					else
					{
						string msg = "Invalid format: " + toString(oArgs);
						throw runtime_error(msg);
					}
				}
				else
				{
					string msg = "Invalid format: " + toString(oArgs);
					throw runtime_error(msg);
				}
			}
		);
	}
}

void FileProcessor::skipOffset(const vector<string> &oArgs)
{
	bool valid;
	m_startPos = parseNumber(oArgs[0], oArgs, valid);
	if (!valid)
	{
		string msg = "Invalid Argument: " + oArgs[0];
		throw runtime_error(msg);
	}
}

void FileProcessor::maxLength(const vector<string> &oArgs)
{
	bool valid;
	m_maxLen = parseNumber(oArgs[0], oArgs, valid);
	if (!valid)
	{
		string msg = "Invalid Argument: " + oArgs[0];
		throw runtime_error(msg);
	}
}

bool FileProcessor::addVectorString(std::vector<uint8_t> &values, const std::string &value, uint16_t size) const
{
	if (value.empty())
		return false;

	bool charmode = false;
	bool first = true;
	bool escaped = false;

	for (char c : value)
	{
		if (first)
		{
			if (c == '\'')
				charmode = true;
			else if (c != '\"')
				return false;

			first = false;

			continue;
		}

		if (escaped)
		{
			switch (c)
			{
				case 'n':
					c = '\n';
				break;
				
				case 'r':
					c = '\r';
				break;

				case 't':
					c = '\t';
				break;
			}

			escaped = false;
		}
		else
		{
			if (c == '\\')
			{
				escaped = true;
				continue;
			}

			if (c == '\"')
				break;
		}

		putVectorValue(values, c, size);

		if(charmode)
			break;
	}

	return true;
}

std::vector<uint8_t> FileProcessor::getVectorData(const std::vector<string> &oData, size_t index) const
{
	vector<uint8_t> values;
	uint16_t size = 8;

	while (index < oData.size())
	{
		string v = oData[index++];

		if (v.empty())
			continue;

		if(addVectorString(values, v, size))
			continue;

		string param;
		parseParam(v,
			[&](const string &par, const string &arg)
			{
				if (arg.empty())
					v = par;
				else
				{
					param = par;
					v = arg;
				}
			}
		);

		if (param == "size")
		{
			size = 8;
			if (!v.empty())
			{
				if (isNumber(v))
					size = fromNumber<uint16_t>(v, nullptr);
				else
					size = 0;
			}
			continue;
		}

		// Value has to be a number
		uint64_t data = fromNumber<uint64_t>(v, nullptr);
		putVectorValue(values, data, size);
	}

	return values;
}

void FileProcessor::writeData(const vector<string> &oArgs)
{
	const vector<uint8_t> &values = getVectorData(oArgs);

	int64_t len = (int64_t)(sizeof(uint8_t)*values.size());
	if (len != 0 && m_output->write(&values[0], len) != len)
	{
		string msg = "Error writing data to file: " + m_output->getOpenpath();
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
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); inputFile(oOption.values().back()); })
	;

	oParser.addOption("output", "o", "Outputfile")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); outputFile(oOption.values().back()); })
	;

	oParser.addOption("data", "d",
R"(Output format type
    [<columns>] [dec[=unsigned(default)|signed]|bin|hex[=cbm(default)|asm|c] [<lineprefix>(default=".byte") <line prefix> <column postfix>]
       <columns> = number of columns per line
       cbm = '$a2', asm = '0a2h', c = '0xa2'
       <lineprefix> = user defined string, default is '.byte')
       <header> = printed first (optional)
       <postfix> = added after the last line (optional)
       <column separator> = printed after each column (optional)
)"
		)
		.arguments(0, 5)
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); dumpData(oOption.values().back()); })
	;

	oParser.addOption("hexdump", "x",
R"(Dumps the file with address and character display
    [columns N:16=default] [dec[=unsigned(default)|signed]|bin|hex[=cbm|asm|c] [ascii=screen|petsci|off] [<addresswidth> = 0|16(defaut)|32|64]
)"
		)
		.arguments(0, 3)
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); dumpHexdump(oOption.values().back()); })
	;

	oParser.addOption("basic", "b",
		R"(Convert to Basic DATA lines
    [columns N:16=default] [dec[=unsigned(default)|signed]] [type=ansi|cbm] [linennumber=N (1000=default)] [stepping=N (10=default)]
)"
		)
		.arguments(0, 6)
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); dumpBasic(oOption.values().back()); })
	;

	oParser.addOption("skip", "s", "Skip first N bytes from inputfile")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); skipOffset(oOption.values().back()); })
	;

	oParser.addOption("length", "l", "Write only N bytes")
		.multiple()
		.arguments()
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); maxLength(oOption.values().back()); })
	;

	oParser.addOption("write", "w", "Write custom data. First value can specify the size of the values [size=8(default)|16|32|64]")
		.multiple()
		.arguments(1, CommandlineParser::UNLIMITED_ARGS)
		.callback([&](CommandlineParser &oParser, const CommandlineParser::Option &oOption) { UNUSED(oParser); writeData(oOption.values().back()); })
	;
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

	if (m_parser.hasArgument("help"))
		return 1;

	m_output->close();

	if (m_result)
		return m_result;

	return 0;
}
