// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"

const char* radioStation = "http://air.radiorecord.ru:8101/rr_320";
DWORD dwBaseSampAddress = NULL;
DWORD dwBaseSampAddressOffset = 0x62DA0;
const char* one = new char(1);

/*
* Working variant, but too much assembler.
void _stdcall startRadioPlay(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4)
{
	if (dwBaseSampAddress == NULL) dwBaseSampAddress = (DWORD)GetModuleHandle(L"samp.dll");
	DWORD funcAddr = dwBaseSampAddress + dwBaseSampAddressOffset;
	_asm
	{
		push dwUnknowParam4
		push fVolumeLevel
		push dwUnknownParam3
		push dwUnknownParam2
		push dwUnknownParam1
		push szURL
		mov ecx, bOne
		call [funcAddr]
	}
}*/
/* No assembler. Working too.*/
void (_stdcall *pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4);

void check()
{
	DWORD addrToCall = NULL;
	while (true)
	{
		if (GetKeyState(VK_F3) < 0)
		{
			if (dwBaseSampAddress == NULL)
			{
				dwBaseSampAddress = (DWORD)GetModuleHandle(L"samp.dll");
				pStartRadioPlay = (void(_stdcall *)(const char*, DWORD, DWORD, DWORD, const float, DWORD))(dwBaseSampAddress + dwBaseSampAddressOffset);
			}
			_asm mov ecx, one//SA-MP checks(compaes with 1) [ecx] and if ecx is unreadable then fault and crash
			pStartRadioPlay(radioStation, 0, 0, 0, 50.0f, 0);
		}
		else if (GetKeyState(VK_F9) < 0)
			break;
		Sleep(15);
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

