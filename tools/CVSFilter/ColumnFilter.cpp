#include "pch.h"

#include "ColumnFilter.h"
#include "utils.h"

using namespace std;

ColumnFilter::ColumnFilter(uint32_t column, bool caseSensitive)
: m_column(column)
, m_caseSensitive(caseSensitive)
{
}

void ColumnFilter::addValue(string value)
{
	if (!isCaseSensitive())
		toUpper(value);

	m_values.push_back(value);
}

bool ColumnFilter::matches(string value)
{
	if (!isCaseSensitive())
		toUpper(value);

	for (const string& val : m_values)
	{
		if (value == val)
			return true;
	}

	return false;
}
