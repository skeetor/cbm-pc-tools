#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"

#include "toolslib/files/MemoryFile.h"

#include "dump.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;
using namespace toolslib::utils;

namespace
{
	template <typename T>
	size_t cmpVector(const vector<T> &first, const vector<T> &second, string &msg)
	{
		size_t index = 0;
		while (index < first.size())
		{
			if (index >= second.size())
			{
				msg = "Size missmatch: " + to_string(first.size()) + " > " + to_string(second.size());
				return index;
			}

			if (first[index] != second[index])
			{
				char v1[4] = { 0 };
				char v2[4] = { 0 };
				sprintf(v1, "$%02x", first[index] & 0xff);
				sprintf(v2, "$%02x", second[index] & 0xff);
				msg = "Comparison misssmatch at [" + to_string(index) + "] " + v1 + " != " + v2;
				return index;
			}

			index++;
		}

		if (second.size() > index)
		{
			msg = "Size missmatch: " + to_string(first.size()) + " < " + to_string(second.size());
			return index;
		}

		return (size_t)-1;
	}

	class TFileProcessor
	: public virtual FileProcessor
	{
	public:
		TFileProcessor(CommandlineParser &parser)
		: FileProcessor(parser)
		, inputFile(0)
		{
		}

		unique_ptr<IFile> createFile(const std::string &oFilename) override
		{
			unique_ptr<IFile> file = make_unique<MemoryFile>(oFilename);

			if (file->getOpenpath() == "input")
			{
				if(!mInputData.empty())
				{
					IFile::open_mode md;
					md.write = true;
					md.read = true;
					md.binary = true;

					file->open(md);
					file->write(&mInputData[inputFile][0], mInputData[inputFile].size());
					file->close();
					mInputFile = file.get();
				}

				inputFile++;
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

		vector<uint8_t> readFile(IFile *file)
		{
			vector<uint8_t> data;

			IFile::open_mode md;
			md.write = false;
			md.read = true;
			md.binary = true;

			file->open(md);
			int64_t len = file->length();
			if (len > 0)
			{
				data.resize(len);
				file->read(&data[0], len);
			}
			file->close();

			return data;
		}

		int inputFile;
		vector<vector<uint8_t>> mInputData;

		IFile *mInputFile;
		IFile *mOutputFile;
	};

	class TestParameter
	{
	public:
		TestParameter(const vector<string>& commandline, const vector<vector<uint8_t>> &input, const vector<uint8_t> &expected)
		: commandLine(commandline)
		, expectedData(expected)
		, inputData(input)
		{
		}

		static vector<uint8_t> stringToVector(const string &str)
		{
			vector<uint8_t> output;

			for (size_t i = 0; i < str.size(); i++)
				output.emplace_back((uint8_t)str[i]);

			return output;
		}

		static string vectorToString(const vector<uint8_t> &vec)
		{
			string str;

			copy(vec.begin(), vec.end(), str.begin());

			return str;
		}

		vector<string> commandLine;
		vector<uint8_t> expectedData;
		vector<vector<uint8_t>> inputData;
	};

	class TDump
		: public ::testing::TestWithParam<TestParameter>
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

		EXPECT_THROW(processor.run(), runtime_error);
		EXPECT_EQ(-1, processor.status());
	}

	// Internal buffer is exactly as big as the requested length
	TEST_F(TDump, BufferExact)
	{
		vector<string> arguments =
		{
			"TEST.EXE", "-o", "output", "-s", "$416", "-l", "$1000", "-i", "input"
		};

		vector<uint8_t> data;
		data.resize(8192);

		uint16_t *p = (uint16_t *)&data[0];
		for (uint16_t i = 0; i < 4096; i++)
			*p++ = i+0x2345;

		vector<uint8_t> expected;
		expected.resize(4096);

		p = (uint16_t *)&expected[0];
		for (uint16_t i = 0; i < expected.size() / 2; i++)
			*p++ = i + 0x2345 +0x416/2;		// Offset from above commandline

		CommandlineParser parser(arguments);
		TFileProcessor processor(parser);
		processor.mInputData.emplace_back(data);

		EXPECT_NO_THROW(processor.run());
		EXPECT_EQ(expected, processor.readOutputFile());
	}

	// Internal buffer is one byte off
	TEST_F(TDump, BufferOneOff)
	{
		vector<string> arguments =
		{
			"TEST.EXE", "-o", "output", "-s", "$416", "-l", "$1001", "-i", "input"
		};

		vector<uint8_t> data;
		data.resize(8192);

		uint16_t *p = (uint16_t *)&data[0];
		for (uint16_t i = 0; i < 4096; i++)
			*p++ = i + 0x2345;

		vector<uint8_t> expected;
		expected.resize(4098);

		p = (uint16_t *)&expected[0];
		for (uint16_t i = 0; i < expected.size()/2; i++)
			*p++ = i + 0x2345 + 0x416 / 2;		// Offset from above commandline
		expected.resize(4097);

		CommandlineParser parser(arguments);
		TFileProcessor processor(parser);
		processor.mInputData.emplace_back(data);

		EXPECT_NO_THROW(processor.run());
		EXPECT_EQ(expected, processor.readOutputFile());
	}

	// Internal buffer is one byte off
	TEST_F(TDump, BufferOneLess)
	{
		vector<string> arguments =
		{
			"TEST.EXE", "-o", "output", "-s", "$416", "-l", "$fff", "-i", "input"
		};

		vector<uint8_t> data;
		data.resize(8192);

		uint16_t *p = (uint16_t *)&data[0];
		for (uint16_t i = 0; i < 4096; i++)
			*p++ = i + 0x2345;

		vector<uint8_t> expected;
		expected.resize(4096);

		p = (uint16_t *)&expected[0];
		for (uint16_t i = 0; i < expected.size() / 2; i++)
			*p++ = i + 0x2345 + 0x416 / 2;		// Offset from above commandline
		expected.resize(4095);

		CommandlineParser parser(arguments);
		TFileProcessor processor(parser);
		processor.mInputData.emplace_back(data);

		EXPECT_NO_THROW(processor.run());
		EXPECT_EQ(expected, processor.readOutputFile());
	}

	INSTANTIATE_TEST_SUITE_P(Parameters, TDump,
		::testing::Values
		(
			// Simply copy a file without any additional processing.
			TestParameter
			(
				  { "TEST.EXE", "-o", "output", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31 } }
				, { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31 }
			)

			// Remove the first two bytes from the source file
			,TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-s", "2", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31 } }
				, { 0x80, 0xff, 0x30, 0x31 }
			)

			// Cut out three bytes from the middle
			,TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-s", "2", "-l", "3", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31 } }
				, { 0x80, 0xff, 0x30 }
			)

			// Copy the the last four bytes and then append another file.
			// Since the length is greater then the input file, the output is
			// the remainder of the input file.
			,TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-w", "16", "$c000", "-s", "2", "-l", "8", "-i", "input", "-i", "input",  }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31 },  { 0x41, 0x42 } }
				,   { 0x00, 0xc0, 0x80, 0xff, 0x30, 0x31, 0x41, 0x42 }
			)

			// Copy the the last four bytes and then append another file.
			// Since the length is greater then the input file, the output is
			// the remainder of the input file.
			, TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-w", "16", "$c000", "-s", "2", "-l", "8", "-i", "input", "input", }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31 },  { 0x41, 0x42 } }
				, { 0x00, 0xc0, 0x80, 0xff, 0x30, 0x31, 0x41, 0x42 }
			)

			// Hex dump with default settings (8 columns, cbm hex)
			,TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "8", "hex", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x32, 0x33, 0x45 } }
				, TestParameter::stringToVector
				(
R"(.byte $00, $7f, $80, $ff, $30, $31, $32, $33
.byte $45
)"				)
			)

			// Hex dump in assembler format (0xxh) with a label
			,TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "8", "hex=asm", ".byte ", "MyLabel:", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x45 } }
				, TestParameter::stringToVector
				(
R"(MyLabel:
.byte 00h, 7fh, 80h, 0ffh, 30h, 31h, 45h
)"				)
			)

			// Dump in C syntax including the variable header
			,TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "8", "hex=c", "", "const char mybuffer[] = {", "};", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x32, 0x33 } }
				, TestParameter::stringToVector
				(
R"(const char mybuffer[] = {
0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x32, 0x33
};
)"				)
			)

			// Dec dump, 7 columns unsigned
			, TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "7", "dec", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x32, 0x33, 0x45 } }
				, TestParameter::stringToVector
				(
R"(.byte 0, 127, 128, 255, 48, 49, 50
.byte 51, 69
)"
				)
			)

			// Dec dump, 7 columns unsigned
			, TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "7", "dec=signed", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x81, 0x31, 0x32, 0x33, 0x45 } }
				, TestParameter::stringToVector
				(
R"(.byte 0, 127, -128, -1, -127, 49, 50
.byte 51, 69
)"
				)
			)

			// Bin dump
			, TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "1", "bin", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0xaa, 0x55 } }
				, TestParameter::stringToVector
				(
R"(.byte %00000000
.byte %01111111
.byte %10000000
.byte %11111111
.byte %10101010
.byte %01010101
)"
				)
			)

			// Hex dump Version 1 using only columns via the data formatter
			, TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "data", "16", "hex", "", "", "", "", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x32, 0x33, 0x45 } }
				, TestParameter::stringToVector
				(
R"($00 $7f $80 $ff $30 $31 $32 $33 $45
)"
				)
			)

			// Hex dump 
			, TestParameter
			(
				{ "TEST.EXE", "-o", "output", "-t", "xd", "16", "hex", "-i", "input" }
				, { { 0x00, 0x7f, 0x80, 0xff, 0x30, 0x31, 0x32, 0x33, 0x45 } }
				, TestParameter::stringToVector
				(
R"($00 $7f $80 $ff $30 $31 $32 $33 $45                             ....0123E
)"
				)
			)
		)
	);

	TEST_P(TDump, Parameters)
	{
		const TestParameter &params = GetParam();

		CommandlineParser parser(params.commandLine);
		TFileProcessor processor(parser);
		processor.mInputData = params.inputData;

		int result = 0xfffffffe;
		EXPECT_NO_THROW((result = processor.run())) << "Params: " << processor.toString(params.commandLine);
		EXPECT_EQ(0, result) << "Params: " << processor.toString(params.commandLine);
		ASSERT_NE(nullptr, processor.mOutputFile) << "Params: " << processor.toString(params.commandLine);
		vector<uint8_t> outData;
		EXPECT_NO_THROW((outData = processor.readOutputFile()));
		size_t index = 0;
		string cmpmsg;
		EXPECT_EQ((size_t)-1, (index = cmpVector(params.expectedData, outData, cmpmsg))) << "Params: " << processor.toString(params.commandLine) << "\n" << cmpmsg;
		EXPECT_EQ((size_t)-1, index);
	}
}
