// CVSFilter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "utils.h"
#include "ColumnFilter.h"
#include "FileStream.h"

using namespace std;

class FileInfo
{
public:
	FileInfo(const string& filename, char separator = ';')
	: m_filename(filename)
	, m_separator(separator)
	{
	}

	FileInfo(const FileInfo& other)
	: m_separator(other.m_separator)
	, m_filename(other.m_filename)
	, m_header(other.m_header)
	{
	}

public:
	char m_separator;
	string m_filename;
	vector<string> m_header;
};

static void help()
{
	cout << "USAGE: CSVColumnFilter\n"
		<< "\t--case 0|1(default)  Enable or disable case sensitivity. Default is on.\n"
		<< "\t[--or]\n"
		<< "\t--column <Column> <Value> <Value> ...\n"
		<< "\t--input <Filename1> <Filename2> ...\n"
		<< "\t--separator <char>    The field separator. Valid for all following inputfiles\n"
		<< "Values for filter on a single column will be 'or'. i.E. --column 0 \"A\" (or) \"B\" ...\n"
		<< "Multiple filters are 'and' by default. I.E. --column 0 a b c (and) --column 1 55 57\n"
		<< "If '--or' is specified, the next filter is 'or'. i.E. --column 0 a b c --or --column 1 55 57\n"
		<< "In the first example, column 0 must contain either a|b|c AND column 1 has 55|57\n"
		<< "In the second example, column 0 must contain either a|b|c or column 1 has 55|57\n"
		<< "If a row matches it is written to stdout.\n"
		<< "Multiple --input can be used and can appear anywhere, filters are always applied to all files.\n"
		<< endl;
}

static int nextParam(int cur, int argc, char*argv[], string& param)
{
	param = "";

	cur++;
	if (cur >= argc)
		return argc;

	param = argv[cur];
	return cur;
}

static bool parseArguments(int argc, char *argv[], vector<FileInfo>& files, vector<vector<ColumnFilter>>& filters)
{
	bool caseSensitivity = true;
	bool andMode = true;
	char separator = ';';

	string param;
	int i = nextParam(0, argc, argv, param);
	if (i == argc)
		return false;

	i = 0;
	while(1)
	{
		i = nextParam(i, argc, argv, param);
		if (i == argc)
			break;

		if (param.length() <= 2)
		{
			cerr << "Invalid argument " << param << endl;
			return false;
		}

		if (param == "--column")
		{
			i = nextParam(i, argc, argv, param);
			if (i == argc)
			{
				cerr << "Missing arguments for --column" << endl;
				return false;
			}

			char *end = nullptr;

			uint32_t column = strtoul(param.c_str(), &end, 10);
			if (*end != 0)
			{
				cerr << "First parameter of --column must be the number of the column: " << param << endl;
				return false;
			}
			ColumnFilter filter(column, caseSensitivity);

			uint32_t count = 0;
			while ((i = nextParam(i, argc, argv, param)) != argc)
			{
				if (param.substr(0, 2) == "--")
				{
					i--;
					break;
				}

				filter.addValue(param);
				count++;
			}

			if (count == 0)
			{
				cerr << "Missing values for --column" << endl;
				return false;
			}

			if (andMode || filters.empty())
			{
				vector<ColumnFilter> row;
				filters.push_back(row);
			}

			vector<ColumnFilter>& row = filters.back();
			row.push_back(filter);

			andMode = true;
		}
		else if (param == "--input")
		{
			uint32_t count = 0;
			while ((i = nextParam(i, argc, argv, param)) != argc)
			{
				if (param.empty())
				{
					cerr << "Inputfilename is empty!" << endl;
					return false;
				}

				if (param.substr(0, 2) == "--")
				{
					i--;
					break;
				}

				FileInfo file(param, separator);
				files.push_back(file);
				count++;
			}

			if (count == 0)
			{
				cerr << "Missing arguments for --input" << endl;
				return false;
			}
		}
		else if (param == "--case")
		{
			i = nextParam(i, argc, argv, param);
			if (i == argc)
			{
				cerr << "Argument missing for --case " << endl;
				return false;
			}

			if (param == "1")
				caseSensitivity = true;
			else if (param == "0")
				caseSensitivity = true;
			else
			{
				cerr << "Invalid argument for --case " << param << endl;
				return false;
			}
		}
		else if (param == "--separator")
		{
			i = nextParam(i, argc, argv, param);
			if (i == argc)
			{
				cerr << "Argument missing for --separator " << endl;
				return false;
			}

			if (param.length() != 1)
			{
				cerr << "Invalid argument for --separator: " << param << endl;
				return false;
			}

			separator = param[0];
		}
		else if (param == "--or")
		{
			andMode = false;
		}
		else if (param == "--help")
		{
			help();
			return false;
		}
		else
		{
			cerr << "Unknown parameter " << param << endl;
			return false;
		}
	}

	if (files.empty())
	{
		cerr << "No input files!" << endl;
		return false;
	}

	return true;
}

static size_t filterFile(FileInfo& fileInfo, StreamReader<string>& reader, const vector<vector<ColumnFilter>>& filters, const vector<uint32_t>& outColumns)
{
	string delim;
	delim += fileInfo.m_separator;

	const vector<string>& header = fileInfo.m_header;

	size_t count = 0;
	size_t lineNr = 1;		// First line is the header so we start at 1
	while (!reader.empty())
	{
		string line = reader.next();
		lineNr++;

		cout << "Line: " << line << endl;

		if (line.empty())
			continue;

		const vector<string>& row = split(line, delim);

		if (row.size() != header.size())
		{
			cerr << "File " << fileInfo.m_filename << ": header doesn't match row " << to_string(row.size()) << " != " << to_string(header.size()) << " at line " << to_string(lineNr) << endl;
			continue;
		}

		count++;
	}

	return count;
}

int main(int argc, char *argv[])
{
	if (argc <= 1)
	{
		help();
		return 0;
	}

	vector<FileInfo> files;
	vector<vector<ColumnFilter>> filters;

	int result = 0;
	if (!parseArguments(argc, argv, files, filters))
		return 1;

	string line;
	vector<uint32_t> outColumns;

	for (FileInfo& fileInfo : files)
	{
		FileStream file(fileInfo.m_filename);

		if (!file.valid())
		{
			cerr << "Unable to read file: " << fileInfo.m_filename << endl;
			return 1;
		}

		if (file.empty())
			continue;

		line = file.next();
		if (line.empty())
			continue;

		string delim;
		delim += fileInfo.m_separator;

		fileInfo.m_header = split(line, delim);
		if (fileInfo.m_header.empty())
			continue;

		if (filterFile(fileInfo, file, filters, outColumns) != -1)
			return 1;
	}

	return 0;
}
