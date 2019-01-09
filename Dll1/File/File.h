#pragma once
#include <string>

using namespace std;

class File
{
protected:
	string filename;

	ios_base::openmode mode;

public:

	File(string filename, ios_base::openmode mode) : filename(filename), mode(mode)
	{}

	File(string filename) : File(filename, (ios::app|ios::in|ios::out))
	{}

	string getFilename()
	{
		return filename;
	}

	ios_base::openmode getMode()
	{
		return mode;
	}

	void setFilename(string &filename)
	{
		this->filename = filename;
	}

	void setMode(ios_base::openmode mode)
	{
		this->mode = mode;
	}
};