#include "gtest/gtest.h"

#include "toolslib/files/MemoryFile.h"

#include "dump.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;
using namespace toolslib::utils;

namespace
{
	class TFileProcessor
	: public virtual FileProcessor
	{
	public:
		TFileProcessor(CommandlineParser &parser)
		: FileProcessor(parser)
		{
		}

		unique_ptr<IFile> createFile(const std::string &oFilename) override
		{
			return make_unique<MemoryFile>(oFilename);
		}
	};

	class TDump
		: public ::testing::Test
	{
	public:
		TDump()
		{
		}

		void SetUp() override
		{
		}

		void TearDown() override
		{
		}

		unique_ptr<FileProcessor> m_dump;
	};

	TEST_F(TDump, NoArguments)
	{
		vector<string> arguments =
		{
			"TEST.EXE"
		};

		CommandlineParser parser(arguments);
		TFileProcessor processor(parser);

		int result = 0xAAAAAAAA;
		EXPECT_NO_THROW((result = processor.run()));
		EXPECT_EQ(-1, result);
	}

	/**
	 * Simply copy a file without any processing.
	 */
	TEST_F(TDump, SimpleCopy)
	{
		vector<string> arguments =
		{
			"TEST.EXE", "-i", "input", "-o", "output"
		};

		CommandlineParser parser(arguments);
		TFileProcessor processor(parser);

		int result = 0xAAAAAAAA;
		EXPECT_NO_THROW((result = processor.run()));
	}
}
