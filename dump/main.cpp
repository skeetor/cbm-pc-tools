// dump.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include <string>
#include <iostream>

#include "toolslib/utils/CommandlineParser.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;

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
	oParser.addOption("input", "i", "Inputfile")
		.mandatory()
		.arguments()
	;

	oParser.addOption("output", "o", "Outputfile")
		.mandatory()
		.arguments()
	;

	oParser.addOption("type", "t", "Output format")
		.mandatory()
		.arguments()
	;

	oParser.addOption("skip", "s", "Skip first N bytes")
		.mandatory()
		.arguments()
	;

	oParser.addOption("length", "l", "Write only N bytes")
		.mandatory()
		.arguments()
		;

	oParser.addOption("address", "a", "Write first two bytes with address.")
		.mandatory()
		.arguments()
		;
}

int main(int argc, char* argv[])
{
	CommandlineParser parser;

	createCommandlineOptions(parser);

	cc65ByteDump(argv[1], 2, -1);

	return 0;
}
