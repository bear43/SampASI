#include "CRadio.h"

const char* one = new char(1);//Just for work. Don't know how to do another way

CRadio::CRadio(string URL) : URL(URL), id(radioStations.size())
{
	radioStations.emplace_back(this);
}

void CRadio::play(const char* URL)
{
	if (startRadioPlay != nullptr)
	{
		startRadioPlay((void*)one, URL, 0, 0, 0, 100.0f, 0);
		isPlaying = true;
	}
}

void CRadio::play(string URL)
{
	play(URL.c_str());
}

void CRadio::stop()
{
	if (channelStop != nullptr && isPlaying)
	{
		channelStop((void*)one, 1);
		isPlaying = false;
	}
}

vector<shared_ptr<CRadio>>& CRadio::getAllInstances()
{
	return radioStations;
}

void CRadio::saveAllInstances()
{
	FileOutput fout(savefile);
	for (shared_ptr<CRadio>& instance : radioStations)
	{
		if(instance != nullptr && !instance->getURL().empty())
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
	startRadioPlay = (pStartRadioPlay)(CSamp::getBaseAddress() + dwOffsetToPlayFunc);
	channelStop = (pChannelStop)(CSamp::getBaseAddress() + dwOffsetToStopFunc);
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

pStartRadioPlay CRadio::startRadioPlay = nullptr;
pChannelStop CRadio::channelStop = nullptr;
vector<shared_ptr<CRadio>> CRadio::radioStations;
bool CRadio::isPlaying = false;
const string CRadio::savefile = "RadioStations.txt";