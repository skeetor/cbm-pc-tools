#include <Windows.h>

#include <algorithm>
#include <string>
#include <cctype>

#include "toolslib/toolslib_def.h"
#include "toolslib/files/Filename.h"
#include "toolslib/string/Helpers.h"
#include "toolslib/string/Wildcards.h"

namespace toolslib
{

namespace files
{

using namespace std;
using namespace toolslib;
using namespace toolslib::strings;

Filename::Filename(const Filename &oSource)
{
	init();
	copy(oSource);
}

Filename::Filename(string const &oFilename, string const &oBasePath)
{
	init();
	setBasePath(oBasePath);
	setFilename(oFilename);
}

Filename::Filename(char const *oFilename, char const *oBasePath)
{
	init();
	setBasePath(oBasePath);
	setFilename(oFilename);
}

void Filename::init(void)
{
	setDelimiter();
}

Filename::~Filename(void)
{
}

Filename &Filename::operator=(const Filename &oSource)
{
	return copy(oSource);
}

Filename &Filename::copy(const Filename &oSource)
{
	mDelimiter = oSource.mDelimiter;
	mBasePath = oSource.mBasePath;
	mFilename = oSource.mFilename;
	mOpenPath = oSource.mOpenPath;

	return *this;
}

void Filename::setBasePath(char const *oBasePath)
{
	if(oBasePath)
		mBasePath = oBasePath;
	else
		mBasePath = "";

	updatePath();
}

string Filename::getBaseDir(void) const
{
	size_t l = mBasePath.size();
	if(l == 0)
		return mBasePath;

	if(mBasePath[l-1] == '\\')
		return mBasePath.substr(0, l-1);

	return mBasePath;
}

void Filename::setFilename(char const *oFilename)
{
	if(oFilename)
		mFilename = oFilename;
	else
		mFilename = "";

	updatePath();
}

void Filename::updatePath(void)
{
	size_t len = mBasePath.size();
	if(len > 0)
	{
		len--;
		if(mBasePath[len] != mDelimiter)
			mBasePath += mDelimiter;
	}

	mOpenPath = mBasePath + mFilename;
}

string Filename::getDrivepath(void) const
{
	return findDrive(getOpenpath());
}

string Filename::findDrive(const string &oPath) const
{
	string drive;

	size_t len = oPath.size();
	char c = static_cast<char>(toupper(oPath[0]));
	if(c >= 'A' && c <= 'Z')
	{
		drive += c;
		drive += ':';
		drive += '\\';

		return drive;
	}
	else if(c == '\\')
	{
		if(len > 0 && oPath[1] == '\\')		// UNC path is ok == Path starts with '\\'
		{
			drive += '\\';
			drive += '\\';

			size_t i = 2;
			bool sharename = false;
			while(i < len)
			{
				c = oPath[i];
				if(c == '\\')
				{
					// The first part is the servername, so we need to get the next part as
					// well, because this donates the accessible path.
					if(sharename == true)
						break;

					sharename = true;
				}

				drive += c;
				i++;
			}

			// The path contains only the servername and no sharename, so we don't have a "drive".
			if(sharename == false)
				drive = "";
			else
			{
				if(drive.size() <= 3)			// Maybe an UNC path or just a bunch of '\'.
					drive = "";
				else
				{
					if(drive[drive.size()-1] != '\\')
						drive += '\\';
				}
			}

			return drive;
		}

		if(drive.size() == 0)
		{
			char path[4096];
			if(GetCurrentDirectory(sizeof(path), path) != 0)
				return findDrive(path);
		}
	}

	return drive;
}

string Filename::getExtension(bool bCase, bool bUppercase) const
{
	string s = getOpenpath();
	size_t l = s.size()-1;

	if(l < 0)
		return "";

	while(l >= 0)
	{
		char c = s[l];
		if(c == '.')
			break;

		if(c == '\\')
			return "";

		l--;
	}

	if(l < 0)
		return "";

	string ext = s.substr(l);

	if(!bCase)
		return ext;

	if(bUppercase)
		transform(ext.begin(), ext.end(), ext.begin(), toUpper);
	else
		transform(ext.begin(), ext.end(), ext.begin(), toLower);

	return ext;
}

string Filename::getFileDir(void) const
{
	string d = getOpenpath();

	size_t l = d.size();
	if(l == 0)
		return d;

	while(--l)
	{
		if(d[l] == '\\')
			break;
	}
	return d.substr(0, l+1);
}

void Filename::normalize(void)
{
	string fn = getOpenpath();
	size_t pos = fn.find_last_of(getDelimiter());
	if(pos == string::npos)
	{
		setBasePath("");
		setFilename(fn);
	}
	else
	{
		setBasePath(fn.substr(0, pos));
		setFilename(fn.substr(pos+1, fn.size()-1));
	}
}

bool Filename::matchesWildcard(const char *pPattern, const char *pString, const char *pEnd, bool bCaseSensitive, bool bAllowEscape)
{
	return strings::matchesWildcard(pPattern, pString, pEnd, bCaseSensitive, bAllowEscape);
}

size_t Filename::findWildcard(const char *pPattern, bool bAllowEscape)
{
	if(!pPattern)
		return invalid;

	size_t index = 0;

	while(*pPattern)
	{
		switch(*pPattern)
		{
			case '*':
			case '?':
			case '[':
				return index;
			break;

			case '\\':
			{
				if(bAllowEscape)
					return index;
			}

			default:
				index++;
			break;
		}

		pPattern++;
	}

	// reached the end of the string without finding a pattern.
	return invalid;
}

}

}
