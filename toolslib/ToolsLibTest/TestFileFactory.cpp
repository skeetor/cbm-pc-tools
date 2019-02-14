#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"

#include "toolslib/files/FileFactory.h"

using namespace std;
using namespace toolslib;
using namespace toolslib::files;

namespace
{
	class TFileFactory
	: public virtual FileFactory
	, public ::testing::Test
	{
	public:
		TFileFactory() {}
	};

	TEST_F(TFileFactory, BasicTests)
	{
		string filename = "dummy.txt";

		unique_ptr<IFile> file(getFile(filename));
		ASSERT_NE(nullptr, file.get());
	}
}
