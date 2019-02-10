#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"

#include "toolslib/files/MemoryFile.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

namespace
{
	class TMemoryFile
	: public virtual MemoryFile
	, public ::testing::Test
	{
	public:
		TMemoryFile() {}
	};

	TEST_F(TMemoryFile, BasicTests)
	{
		string file = "dummy.txt";

		setFilename(file);

		EXPECT_EQ(file, (string)getFilename());
		EXPECT_FALSE(isOpen());
		EXPECT_TRUE(isEOF());
		EXPECT_EQ(invalid64_t, tell());
		EXPECT_EQ(invalid64_t, length());

		EXPECT_TRUE(open());
		EXPECT_TRUE(isOpen());
		EXPECT_EQ(0, length());
		resize(20);
		EXPECT_EQ(20, length());
	}

	TEST_F(TMemoryFile, SeekEmptyFile)
	{
		EXPECT_EQ(invalid64_t, seek(0, IFile::cur));
		EXPECT_EQ(invalid64_t, seek(0, IFile::set));
		EXPECT_EQ(invalid64_t, seek(0, IFile::end));

		EXPECT_TRUE(open());

		EXPECT_EQ(0, seek(0, IFile::cur));
		EXPECT_EQ(0, seek(0, IFile::set));
		EXPECT_EQ(0, seek(0, IFile::end));

		close();
		EXPECT_EQ(invalid64_t, seek(0, IFile::cur));
		EXPECT_EQ(invalid64_t, seek(0, IFile::set));
		EXPECT_EQ(invalid64_t, seek(0, IFile::end));
	}

	TEST_F(TMemoryFile, ReadEmptyFile)
	{
		char buffer[32] = { 0 };
		char cmp[32] = { 0 };

		EXPECT_EQ(invalid64_t, read(buffer, 0));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));

		EXPECT_EQ(invalid64_t, read(buffer, 10));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));

		EXPECT_TRUE(open());

		EXPECT_EQ(0, read(buffer, 0));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));
		EXPECT_EQ(0, read(buffer, 10));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));

		close();
		EXPECT_EQ(invalid64_t, read(buffer, 0));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));
		EXPECT_EQ(invalid64_t, read(buffer, 10));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));
	}

	TEST_F(TMemoryFile, OpenMode)
	{
		char buffer[32] = { 0 };
		char cmp[32] = { 0 };

		// Read only as default
		EXPECT_TRUE(open());
		EXPECT_EQ(0, length());
		EXPECT_EQ(invalid64_t, write(cmp, sizeof(cmp)));
		EXPECT_EQ(0, length());
		close();

		memset(cmp, 0xda, sizeof(cmp));
		IFile::open_mode md = { true,	true,	true,	false,	false,   false };
		EXPECT_TRUE(BaseFile::open(md));
		EXPECT_EQ(0, length());
		EXPECT_EQ((int64_t)sizeof(cmp), write(cmp, sizeof(cmp)));
		EXPECT_EQ((int64_t)sizeof(cmp), length());
		EXPECT_EQ(0, seek(0, IFile::set));
		EXPECT_EQ((int64_t)sizeof(buffer), read(buffer, sizeof(buffer)));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));
		close();

		// reopen the file.
		memset(buffer, 0, sizeof(buffer));
		md = { true,	true,	true,	false,	false,   false };
		EXPECT_TRUE(BaseFile::open(md));
		EXPECT_EQ((int64_t)sizeof(buffer), length());
		EXPECT_EQ((int64_t)sizeof(buffer), read(buffer, sizeof(buffer)));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));
		close();

		md = { true,	true,	true,	false,	false,   true };
		EXPECT_TRUE(BaseFile::open(md));
		EXPECT_EQ(0, length());
	}

	TEST_F(TMemoryFile, ReadWriteFile)
	{
		char buffer[32] = { 0 };
		char cmp[32] = { 0 };

		IFile::open_mode md = { true,	true,	true,	false,	false,   true };

		EXPECT_TRUE(BaseFile::open(md));
		EXPECT_EQ(0, length());

		memset(cmp, 0xda, sizeof(cmp));
		EXPECT_FALSE(isEOF());
		EXPECT_EQ((int64_t)sizeof(cmp), write(cmp, sizeof(cmp)));
		EXPECT_EQ((int64_t)sizeof(cmp), length());
		EXPECT_FALSE(isEOF());

		EXPECT_EQ(0, seek(0, IFile::set));
		EXPECT_EQ((int64_t)sizeof(buffer), read(buffer, sizeof(buffer)));
		EXPECT_FALSE(isEOF());

		EXPECT_EQ(0, read(buffer, sizeof(buffer)));
		EXPECT_TRUE(isEOF());
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));

		EXPECT_EQ(17, seek(-15, IFile::cur));
		EXPECT_EQ(19, seek(2, IFile::cur));
		EXPECT_EQ(13, read(buffer, sizeof(buffer)));
		EXPECT_EQ(31, seek(1, IFile::end));
		EXPECT_EQ(1, read(buffer, sizeof(buffer)));

		int32_t val = 0x1d1d1d1d;
		EXPECT_EQ(14, seek(14, IFile::set));
		EXPECT_EQ((int64_t)sizeof(val), write(&val, sizeof(val)));
		*((int32_t *)&cmp[14]) = val;
		EXPECT_EQ(0, seek(0, IFile::set));
		EXPECT_EQ((int64_t)sizeof(buffer), read(buffer, sizeof(buffer)));
		EXPECT_EQ(0, memcmp(buffer, cmp, sizeof(buffer)));
		EXPECT_EQ(invalid64_t, seek(100, IFile::set));
		EXPECT_EQ(invalid64_t, seek(100, IFile::end));
		EXPECT_EQ(invalid64_t, seek(100, IFile::cur));
	}
}
