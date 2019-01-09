#include "CRadio.h"

const char* one = new char(1);//Just for work. Don't know how to do another way

CRadio::CRadio(string URL) : URL(URL), id(radioStations.size())
{
	radioStations.emplace_back(this);
}

void CRadio::play(const char* URL)
{
	if (pStartRadioPlay != nullptr && !isPlaying)
	{
		_asm push ecx
		_asm mov ecx, one
		pStartRadioPlay(URL, 0, 0, 0, 50.0f, 0);
		_asm pop ecx
		isPlaying = true;
	}
}

void CRadio::play(string URL)
{
	play(URL.c_str());
}

void CRadio::stop()
{
	if (pChannelStop != nullptr && isPlaying)
	{
		_asm push ecx
		_asm mov ecx, one
		pChannelStop(1);
		_asm pop ecx
		isPlaying = false;
	}
}

vector<unique_ptr<CRadio>>& CRadio::getAllInstances()
{
	return radioStations;
}

void CRadio::saveAllInstances()
{
	FileOutput fout(savefile);
	for (unique_ptr<CRadio>& instance : radioStations)
	{
		fout.save((instance->toSaveableData()), true, true, false, false);
	}
	fout.close();
}

void CRadio::loadAllInstances()
{
	stringstream ss;
	FileInput fin(savefile, ios::in);
	for (string& line : fin.readAllLines())
	{
		if (line.empty()) continue;
		ss.str("");
		ss << "Found radio station URL: " << line;
		CSamp::sendMessage(ss.str());
		CRadio *instance = new CRadio(line);
	}
	fin.close();
}

void CRadio::init()
{
	pStartRadioPlay = (void(_stdcall *)(const char*, DWORD, DWORD, DWORD, const float, DWORD))(CSamp::getBaseAddress() + dwOffsetToPlayFunc);
	pChannelStop = (void(_stdcall*)(char))(CSamp::getBaseAddress() + dwOffsetToStopFunc);
}

void CRadio::play()
{
	play(URL);
}

string& CRadio::getURL()
{
	return URL;
}

void CRadio::setURL(string &URL)
{
	this->URL = URL;
}

string CRadio::toSaveableData()
{
	return URL;
}

void CRadio::restore(string &line)
{
	setURL(line);
}

void(_stdcall *CRadio::pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4) = nullptr;
void(_stdcall *CRadio::pChannelStop)(char cUnknownOffset) = nullptr;
vector<unique_ptr<CRadio>> CRadio::radioStations;
bool CRadio::isPlaying = false;
const string CRadio::savefile = "RadioStations.txt";