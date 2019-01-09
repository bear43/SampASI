// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"
#include "FileOutput.h"
#include "FileInput.h"

#define DELAY_TIME 30

const char* one = new char(1);//Just for work. Don't know how to do another way

class CSamp
{
private:

	/* Offset in SA-MP 0.3.7 R3 to connect delay bytes */
	static const DWORD dwOffsetToReconnectDelay = 0x85E2;

	/* Offset in SA-MP 0.3.7 to SAMP_INFO struct */
	static const DWORD dwOffsetToSAMPINFO = 0x26E8DC;

	/* Offset in SA-MP 0.3.7 to chat add func */
	static const DWORD dwOffsetToChatAddFunc = 0x67460;

	static const DWORD dwOffsetToChatInfo = 0x26E8C8;

	/* Chat add func */
	//static void(_stdcall *addToChat)(DWORD dwUnknownParam1, const char message[], DWORD dwUnknownParam2, DWORD color, DWORD dwUnknownParam3);
	static void addToChat(const char message[], DWORD color)
	{
		DWORD funcAddress = dwBaseSampAddress + dwOffsetToChatAddFunc;
		DWORD SAMP_CHAT_POINTER = *((DWORD*)(dwBaseSampAddress + dwOffsetToChatInfo));
		if(funcAddress != dwOffsetToChatAddFunc)
			_asm
			{
				push ecx
				push 0
				push color
				push 0
				push message
				push 8
				push eax
				mov ecx, SAMP_CHAT_POINTER
				pop eax
				call funcAddress
				pop ecx
			}
	}

	/* Base address of samp.dll */
	static DWORD dwBaseSampAddress;

	static bool patched;

public:

	static void setBaseAddres()
	{
		dwBaseSampAddress = (DWORD)GetModuleHandle(L"samp.dll");
	}

	/* Checks base samp.dll address */
	static bool checkBaseAddress()
	{
		return dwBaseSampAddress;
	}

	/* Checks game state. If pointer on the struct of SAMP is null then game is not started yet */
	static bool isGameReady()
	{
		if (!checkBaseAddress()) return false;
		bool *SAMPstruct = (bool*)(dwBaseSampAddress + dwOffsetToSAMPINFO);
		return *SAMPstruct;
	}

	static DWORD getBaseAddress()
	{
		return dwBaseSampAddress;
	}

	/* Decrease reconnect timer */
	static void patchConnectDelayTimer()
	{
		if (!patched)
		{
			unsigned char* addressToPatch = (unsigned char*)(dwBaseSampAddress + dwOffsetToReconnectDelay);
			/* New bytes AA 00 00 00*/
			DWORD oldProtect;
			VirtualProtect(addressToPatch, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
			addressToPatch[1] = 0xAA;
			addressToPatch[2] = 0x00;
			addressToPatch[3] = 0x00;
			addressToPatch[4] = 0x00;
			VirtualProtect(addressToPatch, 5, oldProtect, &oldProtect);
			CSamp::sendMessage("[Decreased connection delay] Byte patched succsessful!");
			patched = true;
		}
	}

	/* Send message to chat */
	static void sendMessage(string message)
	{
		addToChat(message.c_str(), 0xFF00FF00);
	}
};

DWORD CSamp::dwBaseSampAddress = NULL;
bool CSamp::patched = false;

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

	CRadio(string URL) : URL(URL), id(radioStations.size())
	{
		radioStations.emplace_back(this);
	}

	static void play(const char* URL)
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

	static void play(string URL)
	{
		play(URL.c_str());
	}

	static void stop()
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

	static vector<unique_ptr<CRadio>>& getAllInstances()
	{
		return radioStations;
	}

	static void saveAllInstances()
	{
		FileOutput fout(savefile);
		for (unique_ptr<CRadio>& instance : radioStations)
		{
			fout.save((instance->toSaveableData()), true, true, false, false);
		}
		fout.close();
	}
	
	static void loadAllInstances()
	{
		stringstream ss;
		FileInput fin(savefile, ios::in);
		for(string& line : fin.readAllLines())
		{
			if (line.empty()) continue;
			ss.str("");
			ss << "Found radio station URL: " << line;
			CSamp::sendMessage(ss.str());
			CRadio *instance = new CRadio(line);
		}
		fin.close();
	}

	static void init()
	{
		pStartRadioPlay = (void(_stdcall *)(const char*, DWORD, DWORD, DWORD, const float, DWORD))(CSamp::getBaseAddress() + dwOffsetToPlayFunc);
		pChannelStop = (void(_stdcall*)(char))(CSamp::getBaseAddress() + dwOffsetToStopFunc);
	}

	void play()
	{
		play(URL);
	}

	string& getURL()
	{
		return URL;
	}

	void setURL(string &URL)
	{
		this->URL = URL;
	}

	string toSaveableData()
	{
		return URL;
	}

	void restore(string &line)
	{
		setURL(line);
	}

};

void(_stdcall *CRadio::pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4) = 0x0;
void(_stdcall *CRadio::pChannelStop)(char cUnknownOffset) = 0x0;
vector<unique_ptr<CRadio>> CRadio::radioStations;
bool CRadio::isPlaying = false;
const string CRadio::savefile = "RadioStations.txt";

void check()
{
	CSamp::setBaseAddres();
	if (!CSamp::checkBaseAddress()) return;//Exit if no samp.dll
	while (!CSamp::isGameReady())//Very bad. Bydlo code, GOVNO
	{
		Sleep(150);
	}
	CSamp::patchConnectDelayTimer();//Patches delay
	CRadio::init();
	CRadio::loadAllInstances();
	while (true)
	{
		if (GetKeyState(VK_F3) & 0x8000)
		{
			if (GetKeyState(0x35) & 0x8000)
			{
				CRadio::stop();
			}
			else
			{
				CRadio::getAllInstances()[0]->play();
			}
		}
		Sleep(DELAY_TIME);
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, LPTHREAD_START_ROUTINE(check), 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		if(!CRadio::getAllInstances().empty())
			CRadio::saveAllInstances();
        break;
    }
    return TRUE;
}

