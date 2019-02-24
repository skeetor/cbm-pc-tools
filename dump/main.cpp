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

		if (processor.hasHelp())
			return 1;

		result = processor.status();
		if (result)
			return result;

		result = processor.run();
	}
	catch (runtime_error &ex)
	{
		cerr << "Exception: " << ex.what() << endl;
		result = 10;
	}
	catch(...)
	{
		cerr << "Unknown exception!" << endl;
		result = 10;
	}

	return result;
}
