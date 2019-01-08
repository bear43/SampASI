// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"

#define DELAY_TIME 30

const char* radioStation = "http://air.radiorecord.ru:8101/rr_320";//Link to a radio station
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

class CRadio
{
private:
	/* Offset in SA-MP 0.3.7 R3 to radio play func */
	static const DWORD dwOffsetToPlayFunc = 0x661F0;

	/* Offset in SA-MP 0.3.7 R3 to radio stop func */
	static const DWORD dwOffsetToStopFunc = 0x65DF0;

	/* Pointer to samp.dll radio play function */
	static void(_stdcall *pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4);

	/* Pointer to samp.dll channel/free function */
	static void(_stdcall* pChannelStop)(char cUnknownOffset);

	shared_ptr<CSamp> pCSamp = make_shared<CSamp>();

public:

	CRadio(shared_ptr<CSamp> &pCSamp)
	{
		this->pCSamp = pCSamp;
		pStartRadioPlay = (void(_stdcall *)(const char*, DWORD, DWORD, DWORD, const float, DWORD))(pCSamp->getBaseAddress() + dwOffsetToPlayFunc);
		pChannelStop = (void(_stdcall*)(char))(pCSamp->getBaseAddress() + dwOffsetToStopFunc);
	}

	void play(const char* URL)
	{
		if (pStartRadioPlay != nullptr)
		{
			_asm push ecx
			_asm mov ecx, one
			pStartRadioPlay(URL, 0, 0, 0, 50.0f, 0);
			_asm pop ecx
		}
	}

	void play(string URL)
	{
		play(URL.c_str());
	}

	void stop()
	{
		if (pChannelStop != nullptr)
		{
			_asm push ecx
			_asm mov ecx, one
			pChannelStop(1);
			_asm pop ecx
		}
	}
};

void(_stdcall *CRadio::pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4) = 0x0;
void(_stdcall *CRadio::pChannelStop)(char cUnknownOffset) = 0x0;

void check()
{
	shared_ptr<CSamp> samp = make_shared<CSamp>();
	if (!samp->checkBaseAddress()) return;//Exit if no samp.dll
	while (!samp->isGameReady())//Very bad. Bydlo code, GOVNO
	{
		Sleep(150);
	}
	samp->patchConnectDelayTimer();//Patches delay
	shared_ptr<CRadio> radio = make_shared<CRadio>(samp);
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
				radio->play(radioStation);
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

