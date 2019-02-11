#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "toolslib/utils/CommandlineParser.h"

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

		void SetUp() override
		{
			setStrict(true);
		}

		void TearDown() override
		{
		}
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
			"--param", "file2"
		};
		bool parsing = false;
		EXPECT_NO_THROW((parsing = parse(args)));
		EXPECT_FALSE(parsing);
		o = &getUnnamed();
		ASSERT_FALSE(nullptr == o);
		params = &o->values();
		ASSERT_EQ(1u, params->size()) << "Additional parsing";

		values = &(*params)[0];
		ASSERT_EQ(0u, values->size()) << "Additional parsing";

		uint32_t error = getErrorIndex();
		EXPECT_EQ(0u, error);
		EXPECT_EQ("--param", getErrorParam());
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

	TEST_F(TCommandlineParser, ParamterNotGivenRelaxed)
	{
		setStrict(false);

		addOption("enableFeature", "s", "Path to the home directory")
			.optional();
		vector<string> args =
		{
		};

		EXPECT_TRUE(parse(args));
		args =
		{
			"test"
		};
		EXPECT_TRUE(parse(args));
		EXPECT_FALSE(hasArgument("enableFeature"));
	}

	TEST_F(TCommandlineParser, ParamterNotGivenStrict)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.optional();
		vector<string> args =
		{
		};

		EXPECT_TRUE(parse(args));
		args =
		{
			"test"
		};
		EXPECT_FALSE(parse(args));
		EXPECT_FALSE(hasArgument("enableFeature"));
	}

	TEST_F(TCommandlineParser, ParamterWithNoArgs)
	{
		addOption("enableFeature", "s", "Path to the home directory");
		vector<string> args =
		{
			"--enableFeature"
		};

		const vector<vector<string>> *params = nullptr;
		const vector<string> *values = nullptr;

		EXPECT_TRUE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(1u, params->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature")));
		ASSERT_EQ(0u, values->size());
	}

	TEST_F(TCommandlineParser, ParamterWithNoArgsWrongName)
	{
		addOption("enableFeature", "s", "Path to the home directory");
		vector<string> args =
		{
			"--enableFeatures"
		};

		const vector<vector<string>> *params = nullptr;

		EXPECT_FALSE(parse(args));
		EXPECT_THROW((params = &getArguments("enableFeatures")), invalid_argument);
	}

	TEST_F(TCommandlineParser, SingleParamterWithNoArgsMultiple)
	{
		addOption("enableFeature", "s", "Path to the home directory");
		vector<string> args =
		{
			"--enableFeature", "--enableFeature"
		};

		EXPECT_FALSE(parse(args));
		EXPECT_NO_THROW(getArguments("enableFeature"));

		EXPECT_NO_THROW(getArgument("enableFeature", 0));
		EXPECT_NO_THROW(getArgument("enableFeature", 1));
		EXPECT_THROW(getArgument("enableFeature", 2), invalid_argument);
	}

	TEST_F(TCommandlineParser, ParamterWithNoArgsMultiple)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.multiple();
		vector<string> args =
		{
			"--enableFeature", "--enableFeature"
		};

		const vector<vector<string>> *params = nullptr;
		const vector<string> *values = nullptr;

		EXPECT_TRUE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(2u, params->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature")));
		ASSERT_EQ(0u, values->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature", 1)));
		ASSERT_EQ(0u, values->size());

		EXPECT_THROW((values = &getArgument("enableFeature", 2)), invalid_argument);
	}

	TEST_F(TCommandlineParser, ParamterWithSingleMissingArgument)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.arguments();
		vector<string> args =
		{
			"--enableFeature"
		};

		EXPECT_FALSE(parse(args));
	}

	TEST_F(TCommandlineParser, ParamterWithMultipleTooMany1)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.multiple()
			.arguments()
			;
		vector<string> args =
		{
			"--enableFeature", "1", "2"
			,"--enableFeature", "0"
		};

		const vector<vector<string>> *params = nullptr;

		EXPECT_FALSE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(1u, params->size());
	}

	TEST_F(TCommandlineParser, ParamterWithMissingArgument)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.arguments(2, 2)
			;
		vector<string> args =
		{
			"--enableFeature", "1"
		};

		const vector<vector<string>> *params = nullptr;

		EXPECT_FALSE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(1u, params->size());
	}

	TEST_F(TCommandlineParser, ParamterWithMultipleTooManyStrict)
	{
		setStrict(true);
		addOption("enableFeature", "s", "Path to the home directory")
			.multiple()
			.arguments()
			;
		vector<string> args =
		{
			"--enableFeature", "1"
			,"--enableFeature", "0", "2"
		};

		const vector<vector<string>> *params = nullptr;

		EXPECT_FALSE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(2u, params->size());
	}

	TEST_F(TCommandlineParser, ParamterWithMultipleTooMany2)
	{
		// extra values at the end are considered unnamed arguments
		setStrict(false);

		addOption("enableFeature", "s", "Path to the home directory")
			.multiple()
			.arguments()
			;
		vector<string> args =
		{
			"--enableFeature", "1"
			,"--enableFeature", "0", "2"
		};

		const vector<vector<string>> *params = nullptr;
		const vector<string> *values = nullptr;

		EXPECT_TRUE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(2u, params->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature")));
		ASSERT_EQ(1u, values->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature", 1)));
		ASSERT_EQ(1u, values->size());

		EXPECT_THROW((values = &getArgument("enableFeature", 2)), invalid_argument);

		EXPECT_EQ(1u, getUnnamed().values().size());
	}

	TEST_F(TCommandlineParser, ParamterWithMultiple)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.multiple()
			.arguments()
		;
		vector<string> args =
		{
			"--enableFeature", "1"
			,"--enableFeature", "0"
		};

		const vector<vector<string>> *params = nullptr;
		const vector<string> *values = nullptr;

		EXPECT_TRUE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(2u, params->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature")));
		ASSERT_EQ(1u, values->size());
		EXPECT_EQ("1", (*values)[0]);

		EXPECT_NO_THROW((values = &getArgument("enableFeature", 1)));
		ASSERT_EQ(1u, values->size());
		EXPECT_EQ("0", (*values)[0]);

		EXPECT_THROW((values = &getArgument("enableFeature", 2)), invalid_argument);
	}

	TEST_F(TCommandlineParser, ParamterWithMultipleUsingShortcut)
	{
		addOption("enableFeature", "s", "Path to the home directory")
			.multiple()
			.arguments()
			;
		vector<string> args =
		{
			"-s", "1"
			,"-s", "0"
		};

		const vector<vector<string>> *params = nullptr;
		const vector<string> *values = nullptr;

		EXPECT_TRUE(parse(args));
		EXPECT_NO_THROW((params = &getArguments("enableFeature")));
		ASSERT_EQ(2u, params->size());

		EXPECT_NO_THROW((values = &getArgument("enableFeature")));
		ASSERT_EQ(1u, values->size());
		EXPECT_EQ("1", (*values)[0]);

		EXPECT_NO_THROW((values = &getArgument("enableFeature", 1)));
		ASSERT_EQ(1u, values->size());
		EXPECT_EQ("0", (*values)[0]);

		EXPECT_THROW((values = &getArgument("enableFeature", 2)), invalid_argument);
	}

	TEST_F(TCommandlineParser, ParamterWithCallback)
	{
		bool param1 = false;
		bool param2 = false;

		addOption("param1", "", "")
			.multiple()
			.arguments()
			.callback(
				[&](CommandlineParser &oParser, const CommandlineParser::Option &oOption)
				{
					UNUSED(oParser);
					param1 = true;
					const vector<vector<string>> &values = oOption.values();
					EXPECT_EQ(nullptr, oOption.callbackData());
					ASSERT_EQ(1u, values.size());
					ASSERT_EQ(1u, values[0].size());
					EXPECT_EQ("0", values[0][0]);
				}
			)
		;
		addOption("param2", "", "")
			.multiple()
			.arguments(2, 2)
			.callbackData(this)
			.callback(
				[&](CommandlineParser &oParser, const CommandlineParser::Option &oOption)
				{
					UNUSED(oParser);
					param2 = true;
					const vector<vector<string>> values = oOption.values();
					const vector<string> *params = nullptr;

					EXPECT_EQ(this, oOption.callbackData());
					ASSERT_EQ(1u, values.size());

					ASSERT_EQ(2u, values[0].size());
					params = &values[0];
					EXPECT_EQ("1", (*params)[0]);
					EXPECT_EQ("2", (*params)[1]);

					params = &oParser.getArgument("param2");
				}
			)
		;

		vector<string> args =
		{
			"--param1", "0"
			,"--param2", "1", "2"
		};

		EXPECT_TRUE(parse(args));

		EXPECT_TRUE(param1);
		EXPECT_TRUE(param2);
	}
}
