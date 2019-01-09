#pragma once
#include "File.h"
#include <fstream>
#include <vector>

class FileInput : File
{
private:
	ifstream in;

	void checkOpen()
	{
		if (!in.is_open())
			in.open(filename);
	}
public:
	
	FileInput(string filename, _Iosb<int>::_Openmode mode) : File(filename, mode), in(filename, mode)
	{}

	FileInput(string filename) : FileInput(filename, ios::in)
	{}

	string readLine()
	{
		checkOpen();
		string line;
		getline(in, line);
		return line;
	}

	vector<string> readAllLines()
	{
		vector<string> lines;
		while (!in.eof())
		{
			lines.push_back(readLine());
		}
		return lines;
	}

	bool isEnd()
	{
		return in.eof();
	}

	void close()
	{
		if (in.is_open())
			in.close();
	}
};