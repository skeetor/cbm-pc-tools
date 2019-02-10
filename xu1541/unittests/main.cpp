// unittests.cpp : Defines the entry point for the console application.
//

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#pragma comment(lib, "xu1541_dll.lib")

int main(int argc, char *argv[])
{
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS(); 
}
