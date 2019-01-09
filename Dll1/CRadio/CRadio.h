#pragma once
#include "..\CSamp\CSamp.h"
#include <Windows.h>
#include <sstream>
#include <string>
#include "..\File\FileInput.h"
#include "..\File\FileOutput.h"

using namespace std;

class CRadio
{
private:
	/* Saves all radio CRadio instances */
	static vector<unique_ptr<CRadio>> radioStations;

	/* Offset in SA-MP 0.3.7 R3 to radio play func */
	static const DWORD dwOffsetToPlayFunc = 0x661F0;

	/* Offset in SA-MP 0.3.7 R3 to radio stop func */
	static const DWORD dwOffsetToStopFunc = 0x65DF0;

	/* Pointer to samp.dll radio play function */
	static void(_stdcall *pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4);

	/* Pointer to samp.dll channel/free function */
	static void(_stdcall* pChannelStop)(char cUnknownOffset);

	static const string savefile;

	/* Is any radio station plays now? */
	static bool isPlaying;

	int id;

	/* URL to radio station */
	string URL;

public:

	CRadio(string URL);

	static void play(const char* URL);

	static void play(string URL);

	static void stop();

	static vector<unique_ptr<CRadio>>& getAllInstances();

	static void saveAllInstances();

	static void loadAllInstances();

	static void init();

	void play();

	string& getURL();

	void setURL(string &URL);

	string toSaveableData();

	void restore(string &line);

};
