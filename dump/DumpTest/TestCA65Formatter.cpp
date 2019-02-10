#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS

#include "gtest/gtest.h"

#include "ca65Formatter.h"

using namespace std;
using namespace lib::utils;

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
		}

		void TearDown() override
		{
		}
	};

	TEST_F(TCA65Formatter, BasicTests)
	{
		setType(ByteType::HEX);
		EXPECT_EQ(ByteType::HEX, getType());
		setColumns(16);
		EXPECT_EQ(16, getColumns());

		setType(ByteType::BIN);
		EXPECT_EQ(ByteType::BIN, getType());
		setColumns(8);
		EXPECT_EQ(8, getColumns());
	}
}
