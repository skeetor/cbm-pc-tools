// main.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include <stdio.h>

#include "dump.h"

#include "toolslib/utils/CommandlineParser.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;

int main(int argc, char* argv[])
{
	int result = 0;

	try
	{
		CommandlineParser parser(argc, argv);

		FileProcessor processor(parser);

		result = processor.run();

		if (parser.hasArgument("help"))
		{
			parser.help();
			return 1;
		}

	}
	catch (runtime_error &ex)
	{
		if (argc > 1)
		{
			cerr << "Exception: " << ex.what() << endl;
			result = 10;
		}
	}
	catch(...)
	{
		cerr << "Unknown exception!" << endl;
		result = 10;
	}

	return result;
}
