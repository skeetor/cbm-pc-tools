#ifndef STREAMREADER_H
#define STREAMREADER_H

#include <string>

template<typename T>
class StreamReader
{
public:
	virtual ~StreamReader() {}

public:
	/**
	 * Returns true if the stram is valid and can be used.
	 */
	virtual bool valid() = 0;

	/**
	 * Returns false if no more data can be provided.
	 *
	 * @return true|false
	 */
	virtual bool empty() = 0;

	/**
	 * Returns the next value. An empty value is not a sufficient criteria that there is no more 
	 * data available, only if empty() returns true.
	 *
	 * @return url
	 */
	virtual T next() = 0;
};

#endif STREAMREADER_H
