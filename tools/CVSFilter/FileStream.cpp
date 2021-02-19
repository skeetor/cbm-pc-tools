
#include "pch.h"
#include "FileStream.h"

using namespace std;

FileStream::FileStream(const string& file)
{
	m_file = fopen(file.c_str(), "rb");
	m_value.reserve(1024);
}

FileStream::FileStream(FILE* file)
: m_file((file == nullptr) ? stdin : file)
{
	m_value.reserve(1024);
}

FileStream::~FileStream()
{
}

bool FileStream::valid()
{
	return m_file != nullptr;
}

bool FileStream::empty()
{
	if (m_file == nullptr)
	{
		return true;
	}

	// Make sure that feof() is set in case we already reached the end of the file
	ungetc(getc(m_file), m_file);
	if (feof(m_file))
	{
		return true;
	}

	return false;
}

string FileStream::next()
{
	m_value.resize(0);

	if (m_file == nullptr || feof(m_file))
    {
        return m_value;
    }

	char c;
	while (1)
	{
		c = fgetc(m_file);
		if (feof(m_file))
		{
			break;
		}

		if (c == '\r')
		{
			continue;
		}

		if (c == '\n')
		{
			break;
		}

		m_value += c;
	}

	return m_value;
}
