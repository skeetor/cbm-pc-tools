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
			IFile::open_mode md;
			md.write = true;
			md.read = true;
			md.binary = true;

			m_file.open(md);
		}

		void TearDown() override
		{
		}

		MemoryFile m_file;
	};

	TEST_F(TCA65Formatter, DecimalColumnMatch)
	{
		setColumns(4);
		setType(CA65Formatter::DEC);

		char buffer[] = { 0, 1, 2, 3 };
		string expected = ".byte 0, 1, 2, 3\n";
		string written;

		bool success = false;
		EXPECT_NO_THROW((success = format(buffer, sizeof(buffer), &m_file)));
		EXPECT_TRUE(success);
		EXPECT_EQ(expected.length(), (size_t)m_file.length());

		written.resize(m_file.length());
		EXPECT_EQ(0, m_file.seek(0, IFile::set));
		EXPECT_EQ(expected.size(), (size_t)m_file.read(&written[0], written.size()));
		EXPECT_EQ(expected, written);
	}

	TEST_F(TCA65Formatter, DecimalColumnOverlap)
	{
		setColumns(4);
		setType(CA65Formatter::DEC);

		char buffer[] = { 0, 1, 2, 3, 4};
		string expected = ".byte 0, 1, 2, 3\n.byte 4\n";
		string written;

		bool success = false;
		EXPECT_NO_THROW((success = format(buffer, sizeof(buffer), &m_file)));
		EXPECT_TRUE(success);
		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);
		EXPECT_EQ(expected.length(), (size_t)m_file.length());

		written.resize(m_file.length());
		EXPECT_EQ(0, m_file.seek(0, IFile::set));
		EXPECT_EQ(expected.size(), (size_t)m_file.read(&written[0], written.size()));
		EXPECT_EQ(expected, written);
	}

	TEST_F(TCA65Formatter, DecimalColumnRandom)
	{
		setColumns(4);
		setType(CA65Formatter::DEC);

		char buffer[] = { 0, 1, 2, 3, 4 };
		string expected = ".byte 0, 1, 2, 3\n.byte 4\n";
		string written;

		bool success = false;
		for (char *p = buffer; p < &buffer[sizeof(buffer)]; p++)
		{
			EXPECT_NO_THROW((success = format(p, sizeof(*buffer), &m_file)));
			EXPECT_TRUE(success);
		}

		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);

		// Make sure that multiple flushes have no effect
		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);
		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);

		EXPECT_EQ(expected.length(), (size_t)m_file.length());

		written.resize(m_file.length());
		EXPECT_EQ(0, m_file.seek(0, IFile::set));
		EXPECT_EQ(expected.size(), (size_t)m_file.read(&written[0], written.size()));
		EXPECT_EQ(expected, written);
	}

#pragma warning(push)
#pragma warning(disable: 4310 )

	TEST_F(TCA65Formatter, DecimalColumnMultiple)
	{
		setColumns(4);
		setType(CA65Formatter::DEC);

		char buffer[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, (char)0xff };
		string expected = ".byte 0, 1, 2, 3\n.byte 4, 5, 6, 7\n.byte 8, 9, 10, 11\n.byte 12, 255\n";
		string written;

		bool success = false;
		for (char *p = buffer; p < &buffer[sizeof(buffer)]; p += 2)
		{
			EXPECT_NO_THROW((success = format(p, sizeof(*buffer)*2, &m_file)));
			EXPECT_TRUE(success);
		}

		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);

		// Make sure that multiple flushes have no effect
		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);
		EXPECT_NO_THROW((success = flush(&m_file)));
		EXPECT_TRUE(success);

		EXPECT_EQ(expected.length(), (size_t)m_file.length());

		written.resize(m_file.length());
		EXPECT_EQ(0, m_file.seek(0, IFile::set));
		EXPECT_EQ(expected.size(), (size_t)m_file.read(&written[0], written.size()));
		EXPECT_EQ(expected, written);
	}
#pragma warning(pop)

}
