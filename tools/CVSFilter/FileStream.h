#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <string>
#include <stdio.h>

#include "StreamReader.h"

class FileStream
: public StreamReader<std::string>
{
public:
	FileStream(FILE* file = nullptr);
	FileStream(const std::string& file);
	~FileStream() override;

public:
	bool valid() override;
	bool empty() override;
	std::string next() override;

private:
	FILE *m_file;
	std::string m_value;
};

#endif FILESTREAM_H
