#pragma once

#ifndef _COLUMNFILTER_H
#define _COLUMNFILTER_H

#include <vector>
#include <string>

class ColumnFilter
{
public:
	ColumnFilter(uint32_t column, bool caseSensitive = true);

	void setCaseSensitive(bool caseSensitive)
	{
		m_caseSensitive = caseSensitive;
	}

	bool isCaseSensitive() const
	{
		return m_caseSensitive;
	}

	void setColumnId(uint32_t column)
	{
		m_column = column;
	}

	uint32_t getColumnId() const
	{
		return m_column;
	}

	void addValue(std::string value);
	bool matches(std::string value);

private:
	uint32_t m_column;
	bool m_caseSensitive;
	std::vector<std::string> m_values;
};

#endif _COLUMNFILTER_H
