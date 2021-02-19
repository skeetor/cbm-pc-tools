#ifdef _WIN32
#include "pch.h"
#endif

#include <iostream>
#include <string>
#include <cstdlib>
#include <set>
#include <vector>
#include <iomanip>
#include <sstream>
#include <random>
#include <cstring>
#include <fstream>

#include <ctime>
#include <time.h>

#ifdef _WIN32
#define timegm _mkgmtime
#endif

using namespace std;

static mt19937 createGenerator()
{
	random_device seed;

	mt19937 generator(seed());

	return generator;
}

mt19937 generator = createGenerator();

static const string timestampStr = "<TIMESTAMP>";
static const string hashStr = "<HASH>";
static const string lineStr = "<RECORD>";

static size_t rndRange(size_t minVal, size_t maxVal)
{
	uniform_int_distribution<size_t> distr(minVal, maxVal);

	return distr(generator);
}

static string createRandomString(uint32_t maxLen, const string &charset = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")
{
	string s;

	while (s.size() < maxLen)
	{
		size_t val = rndRange(0, charset.length()-1);

		s += charset[val];
	}

	return s;
}

static void help(void)
{
	cout 
		<< "USAGE:\n"
		<< "--length N (default = 64)\n"
		<< "--count N (default = 1000000)\n"
		<< "--interval <timestamp|YYYY.MM.DD HH:MI:SS> <timestamp|YYYY.MM.DD HH:MI:SS>\n"
		<< "--timeformat <FormatString> (default is UNIX time in ms)\n"
		<< "--file <filename> read a file containing hashes, instead of randomly generating them.\n"
		<< "--nototal If count is specified with file then <nototal> will takne N records from each file\n"
		<< "\tinstead of a total of N records.\n"
		<< "--split <string> <position>\n"
		<< "--out string (default is '<HASH>')\n"
		<< "\tThis may contain the following template values:\n"
		<< "\t'<HASH>' Put a random hashvalue\n"
		<< "\t'<TIMESTAMP>' If an intervall is given, then the string is replaced with a randomized timestamp.\n"
		<< "\t'<RECORD>' Print the recordnumber\n"
	<< endl;
}

static vector<string> split(const string& s, string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	string token;
	vector<string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}
static string sreplace(string subject, const string& search, const string& replace)
{
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != string::npos)
	{
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}

	return subject;
}

static int FindParam(int argc, char** argv, const char* name, vector<string>& params, int start = 1)
{
	params.clear();

	char par = false;
	size_t nameLen = strlen(name);
	int i = 0;

	for (i = start; i < argc; i++)
	{
		const char* p = argv[i];

		if (!par)
		{
			size_t len = strlen(p);

			if (len <= 2)
			{
				if (nameLen > 2)
					continue;
			}

			if (p[0] == '-')
				p++;

			if (p[0] == '-')
				p++;

			if (strcmp(p, name) == 0)
				par = true;

			continue;
		}

		if (p[0] == '-')
			break;

		params.push_back(p);
	}

	if (!par)
		return -1;

	return i;
}

#ifdef _WIN32
extern "C" char* strptime(const char* s, const char* f, struct tm* tm)
{
	// Isn't the C++ standard lib nice? get_time is defined such that its
	// format parameters are the exact same as strptime. Of course, we have to
	// create a string stream first, and imbue it with the current C locale, and
	// we also have to make sure we return the right things if it fails, or
	// if it succeeds, but this is still far simpler an implementation than any
	// of the versions in any of the C standard libraries.
	istringstream input(s);
	input.imbue(locale(setlocale(LC_ALL, nullptr)));
	input >> get_time(tm, f);
	if (input.fail())
		return nullptr;

	return (char*)(s + input.tellg());
}
#endif

static size_t strToEpoch(const string& timeStr, const vector<string> formats, bool* isEpoch = nullptr)
{
	bool fake;

	if (!isEpoch)
		isEpoch = &fake;

	char* charpos = nullptr;
	size_t t = strtoull(timeStr.c_str(), &charpos, 10);
	if (*charpos == 0)
	{
		*isEpoch = true;
		return t;
	}

	*isEpoch = false;

	bool ok = false;
	struct tm tp = { 0 };
	for (string format : formats)
	{
		struct tm tt = { 0 };
		const char* pos = strptime(timeStr.c_str(), format.c_str(), &tt);
		if (pos == nullptr)
		{
			continue;
		}

		tp = tt;
		ok = true;
		break;
	}

	if (!ok)
	{
		cerr << "[ERR] Unknown time format for " << timeStr << endl;
		cerr << "Allowed formats are:" << endl;
		for (string format : formats)
		{
			cerr << "\t" << format << endl;
		}

		return (size_t)-1;
	}

	// To UTC.
	t = timegm(&tp);
	if (t == (size_t)-1)
	{
		return -1;
	}

	return t * 1000;
}

static string epochToString(size_t timestamp, const string& format = "%Y.%m.%d %H:%M:%S")
{
	time_t tm = timestamp / 1000;

	struct tm* t = gmtime(&tm);
	if (!t)
	{
		return "";
	}

	char buffer[200];
	char* p = buffer;
	size_t len = sizeof(buffer);
	size_t v;

	while ((v = strftime(p, len, format.c_str(), t)) == 0)
	{
		if (p != buffer)
			delete[] p;

		len += 200;
		p = new char[len];
	}

	string s = p;

	if (p != buffer)
		delete[] p;

	return s;
}

static size_t generateTimestamp(size_t start, size_t end)
{
	return rndRange(start, end);
}

int main(int argc, char *argv[])
{
	const string hex = "0123456789abcdef";

	uint32_t hashLen = 64;
	size_t count = 1000000;
	string outParam;
	string timeFormat;
	vector<string> hashFiles;
	char *e;
	size_t intervalStart = -1;
	size_t intervalEnd = -1;
	unique_ptr<istream> fileStream;
	istream* strm = nullptr;
	bool fromFile = false;
	bool total = true;
	string pattern;
	size_t patternField = 0;

	vector<string> params;
	if (FindParam(argc, argv, "help", params) != -1 || FindParam(argc, argv, "h", params) != -1)
	{
		help();
		return 0;
	}

	if (FindParam(argc, argv, "length", params) != -1)
	{
		if (params.size() != 1)
		{
			cerr << "--length requires a numeric value" << endl;
			return 10;
		}

		hashLen = strtol(params[0].c_str(), &e, 10);
	}

	if (FindParam(argc, argv, "out", params) != -1)
	{
		if (params.size() != 1)
		{
			cerr << "--prefix requires a string value" << endl;
			return 10;
		}

		outParam = params[0];
	}
	else
		outParam = "<HASH>";

	if (FindParam(argc, argv, "interval", params) != -1)
	{
		if (params.size() != 2)
		{
			cerr << "--interval requires two values, either as numeric timestmap or as date 'YYYY.MM.DD HH:MI:SS'" << endl;
			return 10;
		}

		static const vector<string> formats =
		{
			 "%Y.%m.%d %H:%M:%S"
			,"%Y-%m-%d %H:%M:%S"
			,"%Y/%m/%d %H:%M:%S"
		};

		intervalStart = strToEpoch(params[0], formats);
		intervalEnd = strToEpoch(params[1], formats);
	}

	if (FindParam(argc, argv, "timeformat", params) != -1)
	{
		if (params.size() != 1)
		{
			cerr << "--timeformat requires a string value" << endl;
			return 10;
		}

		timeFormat = params[0];
	}

	if (FindParam(argc, argv, "file", hashFiles) != -1)
	{
		if (hashFiles.size() < 1)
		{
			cerr << "--file requires at least one path or 'stdin'" << endl;
			return 10;
		}

		count = 0;
		fromFile = true;
	}

	if (FindParam(argc, argv, "count", params) != -1)
	{
		if (params.size() != 1)
		{
			cerr << "--count requires a numeric value" << endl;
			return 10;
		}

		count = strtol(params[0].c_str(), &e, 10);
	}

	if (FindParam(argc, argv, "nototal", params) != -1)
	{
		total = false;
	}

	if (FindParam(argc, argv, "split", params) != -1)
	{
		if (params.size() != 2)
		{
			cerr << "--split requires a string and a position" << endl;
			return 10;
		}

		pattern = params[0];
		patternField = strtol(params[1].c_str(), &e, 10);
	}

	size_t i = 0;
	size_t record = 0;
	filebuf fb;

	// Either produce hashes until the max count is reached, or the end of the inputfile.
	while (i < count || fromFile)
	{
		string hash;

		if (!fromFile)
			hash = createRandomString(hashLen, hex);
		else
		{
			if (strm)
				getline(*strm, hash);

			if (!strm || !strm->good() || i >= count)
			{
				if (hashFiles.empty())
					break;

				string hashFile = hashFiles[0];
				hashFiles.erase(hashFiles.begin());

				if (hashFile == "stdin)")
					strm = &cin;
				else
				{
					fb.close();

					if (!fb.open(hashFile, ios::in))
					{
						cerr << "Unable to open " << hashFile << endl;
						return 10;
					}

					fileStream = make_unique<istream>(&fb);
					strm = fileStream.get();
					getline(*strm, hash);
				}

				// If we want to take N records from each file, we have to reset the record counter.
				if (!total)
					i = 0;
			}
		}

		if (!pattern.empty())
		{
			vector<string> fields = split(hash, pattern);
			if (patternField >= fields.size())
			{
				cerr << hash << " doesn't contain the splitpattern " << pattern  << endl;
				continue;
			}

			hash = fields[patternField];
		}

		string out = outParam;

		if (intervalStart != (size_t)-1)
		{
			size_t timestamp = generateTimestamp(intervalStart, intervalEnd);
			string ts;

			if (timeFormat.empty())
				ts = to_string(timestamp);
			else
				ts = epochToString(timestamp, timeFormat);

			out = sreplace(out, timestampStr, ts);

			/*cout 
				<< "Start:" << epochToString(intervalStart) 
				<< " End:" << epochToString(intervalEnd)
				<< " Timestamp:" << epochToString(timestamp)
			<< endl;*/
		}

		out = sreplace(out, hashStr, hash);
		out = sreplace(out, lineStr, to_string(++record));

		cout << out << endl;

		i++;
	}

	return 0;
}
