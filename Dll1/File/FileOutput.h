#pragma once
#include <fstream>
#include <vector>
#include "File.h"

using namespace std;

#define DEFAULT_NEXTLINE true
#define DEFAULT_FLUSH true
#define DEFAULT_CLOSE true
#define DEFAULT_EMTYLINES true

class FileOutput : File
{
private:
	ofstream out;
	void checkOpen()
	{
		if (!out.is_open())
			out.open(filename, mode);
	}
public:
	FileOutput(string filename, ios_base::openmode mode) : File(filename, mode), out(filename, mode)
	{}
	FileOutput(string filename) : FileOutput(filename, ios::out)
	{}
	~FileOutput()
	{
		if (out.is_open())
			out.flush();
		out.close();
	}

	bool save(string line, bool nextline, bool flush, bool close, bool emptylines)
	{
		if (line.empty() && !emptylines) return true;
		checkOpen();
		if (!out.is_open()) return false;
		out << line;
		if (nextline) out << endl;
		if (flush) out.flush();
		if (close) out.close();
		return true;
	}

	bool save(string line, bool nextline, bool flush)
	{
		return save(line, nextline, flush, DEFAULT_CLOSE, DEFAULT_EMTYLINES);
	}

	bool save(string line, bool nextline)
	{
		return save(line, nextline, DEFAULT_FLUSH);
	}

	bool save(string line)
	{
		return save(line, DEFAULT_NEXTLINE);
	}

	bool save(vector<string> &lines)
	{
		for (string &line : lines)
			if (!save(line))
			{
				return false;
			}
	}

	bool flush(bool close)
	{
		if (!out.is_open()) return false;
		out.flush();
		if (close) out.close();
		return true;
	}

	bool flush()
	{
		return flush(DEFAULT_CLOSE);
	}

	void close()
	{
		if (out.is_open())
			out.close();
	}
};