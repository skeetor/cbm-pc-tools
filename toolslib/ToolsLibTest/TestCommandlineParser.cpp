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

	TEST_F(TCommandlineParser, BasicTests)
	{
		const vector<CommandlineParser::Option>& options = getOptions();
		ASSERT_EQ(1u, options.size());
		const CommandlineParser::Option &option = options[0];
		EXPECT_EQ("", option.name());
		EXPECT_EQ("", option.param());

		EXPECT_THROW(addOption("", "", "Missing names"), invalid_argument);
		EXPECT_EQ(1u, options.size());

		EXPECT_EQ(nullptr, findParam("test1"));
		EXPECT_EQ(nullptr, findParam("test2"));

		const vector<vector<string>> *params;
		const vector<string> *values;
		const CommandlineParser::Option *o;

		vector<string> args =
		{
			"file1"
		};
		EXPECT_NO_THROW(parse(args));
		o = &getUnnamed();
		ASSERT_FALSE(nullptr == o);
		params = &o->values();
		ASSERT_EQ(1u, params->size()) << "Initial parsing";
		values = &(*params)[0];
		ASSERT_EQ(1u, values->size()) << "Initial parsing";
		EXPECT_EQ("file1", (*values)[0]);

		// Calling parse a second time will reset any previously parsed parameters
		args =
		{
			"--param file1"
		};
		EXPECT_NO_THROW(parse(args));
		o = &getUnnamed();
		ASSERT_FALSE(nullptr == o);
		params = &o->values();
		ASSERT_EQ(1u, params->size()) << "Additional parsing";
		values = &(*params)[0];
		ASSERT_EQ(1u, values->size()) << "Additional parsing";
		EXPECT_EQ("--param file1", (*values)[0]);
	}

	TEST_F(TCommandlineParser, SingleOption)
	{
		EXPECT_NO_THROW(addOption("settings", "s", "Path to the home directory"));
		const vector<CommandlineParser::Option>& options = getOptions();
		ASSERT_EQ(2u, options.size());
		const CommandlineParser::Option &option = options[1];
		EXPECT_EQ("settings", option.name());
		EXPECT_EQ("s", option.param());
	}

	TEST_F(TCommandlineParser, SingleUnnamedParameter)
	{
	}
}
