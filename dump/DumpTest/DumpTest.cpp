#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#pragma comment(lib, "toolslib.lib")

int main(int argc, char *argv[])
{
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	int rc = RUN_ALL_TESTS();

	return rc;
}
