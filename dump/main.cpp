// dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <string>
#include <iostream>

#include "toolslib/utils/CommandlineParser.h"
#include "toolslib/files/MemoryFile.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;
using namespace toolslib::files;

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

static void createCommandlineOptions(CommandlineParser &oParser)
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
	;

	oParser.addOption("output", "o", "Outputfile")
		.mandatory()
		.arguments()
	;

	oParser.addOption("type", "t", "Output format")
		.arguments()
	;

	oParser.addOption("skip", "s", "Skip first N bytes")
		.arguments()
	;

	oParser.addOption("length", "l", "Write only N bytes")
		.arguments()
		;

	oParser.addOption("address", "a", "Write first two bytes with address.")
		.arguments()
		;
}

int main(int argc, char* argv[])
{
	MemoryFile mf;

	CommandlineParser parser;

	createCommandlineOptions(parser);

	if (!parser.parse(argc, argv))
	{
		uint32_t error = parser.getErrorIndex();
		cerr << "Error with parameter '" << parser.getErrorParam() << "' at position " << to_string(error) << endl;
		return -1;
	}

	if (parser.hasArgument("help"))
	{
		parser.help();
		return 1;
	}

	cc65ByteDump(argv[1], 2, -1);

	return 0;
}
