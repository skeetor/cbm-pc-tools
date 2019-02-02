#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "utils/CommandlineParser.h"

#pragma comment(lib, "toolslib.lib")

using namespace std;
using namespace toolslib;
using namespace toolslib::utils;

namespace
{
	class TCommandlineParser
	: public CommandlineParser
	, public ::testing::Test
	{
	public:
		TCommandlineParser() {}
	};

	TEST_F(TCommandlineParser, Construction)
	{
		const vector<CommandlineParser::Option>& options = getOptions();
		EXPECT_EQ(1u, options.size());
	}

	TEST_F(TCommandlineParser, SingleParam)
	{
		EXPECT_NO_THROW(addOption("settings", "Path to the home directory"));
		const vector<CommandlineParser::Option>& options = getOptions();
	}
}
