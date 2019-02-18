#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"

#include "toolslib/files/MemoryFile.h"
#include "formatter/CA65Formatter.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

namespace
{
	class TCA65Formatter
	: public ::testing::Test
	, public CA65Formatter
	{
	public:
		TCA65Formatter()
		{
		}

		void SetUp() override
		{
			m_file.open();
		}

		void TearDown() override
		{
		}

		MemoryFile m_file;
	};

	TEST_F(TCA65Formatter, DecimalColumnMatch)
	{
		setColumns(8);

		char buffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
		string expected = ".byte 0, 1, 2, 3, 4, 5, 6, 7, 8";
		string written;

		bool success = false;
		EXPECT_NO_THROW((success = format(buffer, sizeof(buffer), &m_file, false)));
		EXPECT_TRUE(success);
		EXPECT_EQ(expected.size(), (size_t)m_file.length());

		written.resize(m_file.length());
		EXPECT_EQ(0, m_file.seek(0, IFile::set));
		EXPECT_EQ(expected.size(), (size_t)m_file.read(&written[0], written.size()));
	}
}
