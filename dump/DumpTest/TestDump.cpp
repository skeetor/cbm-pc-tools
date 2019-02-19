#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"

#include "toolslib/files/MemoryFile.h"

#include "dump.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

namespace
{
	class TFileProcessor
	: public virtual FileProcessor
	{
	public:
		TFileProcessor(toolslib::utils::CommandlineParser &parser)
		: FileProcessor(parser)
		{
		}

		toolslib::files::IFile *createFile(const std::string &oFilename) override
		{
			return new MemoryFile(oFilename);
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

	TEST_F(TDump, Base)
	{
	}
}
