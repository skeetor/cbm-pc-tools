// main.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

#include "dump.h"

#include "toolslib/utils/CommandlineParser.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;

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

	return result;
}
