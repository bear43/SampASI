// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"
//#include "FileOutput.h"

#define DELAY_TIME 30

const string radioStation = "http://air.radiorecord.ru:8101/rr_320";//Link to a radio station
const char* one = new char(1);//Just for work. Don't know how to do another way

class CSamp
{
private:

	/* Offset in SA-MP 0.3.7 R3 to connect delay bytes */
	static const DWORD dwOffsetToReconnectDelay = 0x85E2;

	/* Offset in SA-MP 0.3.7 to SAMP_INFO struct */
	static const DWORD dwOffsetToSAMPINFO = 0x26E8DC;

	/* Base address of samp.dll */
	DWORD dwBaseSampAddress = NULL;

	bool patched = false;

public:
	CSamp() : dwBaseSampAddress((DWORD)GetModuleHandle(L"samp.dll"))
	{}

	/* Checks base samp.dll address */
	bool checkBaseAddress()
	{
		return dwBaseSampAddress;
	}

	/* Checks game state. If pointer on the struct of SAMP is null then game is not started yet */
	bool isGameReady()
	{
		if (!checkBaseAddress()) return false;
		bool *SAMPstruct = (bool*)(dwBaseSampAddress + dwOffsetToSAMPINFO);
		return *SAMPstruct;
	}

	DWORD getBaseAddress()
	{
		return dwBaseSampAddress;
	}

	/* Decrease reconnect timer */
	void patchConnectDelayTimer()
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
			patched = true;
		}
	}
};

class CRadio : Saveable<CRadio>
{
private:
	/* Saves all radio CRadio instances */
	static vector<CRadio*> radioStations;

	/* Offset in SA-MP 0.3.7 R3 to radio play func */
	static const DWORD dwOffsetToPlayFunc = 0x661F0;

	/* Offset in SA-MP 0.3.7 R3 to radio stop func */
	static const DWORD dwOffsetToStopFunc = 0x65DF0;

	/* Pointer to samp.dll radio play function */
	static void(_stdcall *pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4);

	/* Pointer to samp.dll channel/free function */
	static void(_stdcall* pChannelStop)(char cUnknownOffset);

	static shared_ptr<CSamp> pCSamp;

	/* Is any radio station plays now? */
	static bool isPlaying;

	int id;

	/* URL to radio station */
	string URL;

public:

	CRadio(shared_ptr<CSamp> &pCSamp, string &URL) : URL(URL)
	{
		if (!this->pCSamp)
		{
			this->pCSamp = pCSamp;
			pStartRadioPlay = (void(_stdcall *)(const char*, DWORD, DWORD, DWORD, const float, DWORD))(pCSamp->getBaseAddress() + dwOffsetToPlayFunc);
			pChannelStop = (void(_stdcall*)(char))(pCSamp->getBaseAddress() + dwOffsetToStopFunc);
		}
	}

	CRadio(string &URL) : URL(URL), id(radioStations.size())
	{}

	~CRadio()
	{
		if (!radioStations.empty())
		{
			auto it = find(radioStations.begin(), radioStations.end(), this);
			if (it != radioStations.end())
			{
				radioStations.erase(it);
			}
		}
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

	static vector<CRadio*>& getAllInstances()
	{
		return radioStations;
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

	vector<CRadio*> getStations()
	{
		return radioStations;
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
shared_ptr<CSamp> CRadio::pCSamp = nullptr;
vector<CRadio*> CRadio::radioStations;
bool CRadio::isPlaying = false;

void check()
{
	shared_ptr<CSamp> samp = make_shared<CSamp>();
	if (!samp->checkBaseAddress()) return;//Exit if no samp.dll
	while (!samp->isGameReady())//Very bad. Bydlo code, GOVNO
	{
		Sleep(150);
	}
	samp->patchConnectDelayTimer();//Patches delay
	shared_ptr<CRadio> radio = make_shared<CRadio>(samp, (string&)radioStation);
	while (true)
	{
		if (GetKeyState(VK_F3) & 0x8000)
		{
			if (GetKeyState(0x35) & 0x8000)
			{
				radio->stop();
			}
			else
			{
				radio->play();
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
        break;
    }
    return TRUE;
}

