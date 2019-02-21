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
			unique_ptr<IFile> file = make_unique<MemoryFile>(oFilename);

			if (file->getOpenpath() == "input" && !mInputData.empty())
			{
				IFile::open_mode md;
				md.write = true;
				md.read = true;
				md.binary = true;

				file->open(md);
				file->write(&mInputData[0], mInputData.size());
				file->close();

				mInputFile = file.get();
			}
			else
				mOutputFile = file.get();

			return file;
		}

		vector<uint8_t> readInputFile(void)
		{
			return readFile(mInputFile);
		}

		vector<uint8_t> readOutputFile(void)
		{
			return readFile(mOutputFile);
		}

		vector<uint8_t> readFile(unique_ptr<IFile>& file)
		{
			vector<uint8_t> data;

			IFile::open_mode md;
			md.write = false;
			md.read = true;
			md.binary = true;

			file->open(md);
			int64_t len = file->length();
			data.resize(len);
			file->read(&data[0], len);
			file->close();

			return data;
		}

		vector<uint8_t> mInputData;

		IFile *mInputFile;
		IFile *mOutputFile;
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
		vector<uint8_t> data =
		{
			0x00, 0x7f, 0x80, 0xff
		};

		CommandlineParser parser(arguments);
		TFileProcessor processor(parser);
		processor.mInputData = data;

		int result = 0xAAAAAAAA;
		EXPECT_NO_THROW((result = processor.run()));
		EXPECT_EQ(0, result);
		EXPECT_EQ(data, processor.readOutputFile());
	}
}
