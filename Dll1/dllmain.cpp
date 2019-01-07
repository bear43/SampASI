// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "stdafx.h"

const char* radioStation = "http://air.radiorecord.ru:8101/rr_320";//Link to a radio station
DWORD dwBaseSampAddress = NULL;//Base address of samp.dll
//DWORD dwBaseSampAddressOffset = 0x62DA0;//Offset in samp.dll to radio function SA-MP 0.3.7 R1
const DWORD dwBaseSampAddressOffsetToPlayFunc = 0x661F0;//Offset in SA-MP 0.3.7 R3
const DWORD dwBaseSampAddressOffsetToStopFunc = 0x65DF0;//Offset in SA-MP 0.3.7 R3
const char* one = new char(1);//Just for work. Don't know how to do another way

/*
* Working fine, but too much assembler.
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
		mov ecx, one
		call [funcAddr]
	}
}*/
/* No assembler. Working too. May be not always.*/
void(_stdcall *pStartRadioPlay)(const char* szURL, DWORD dwUnknownParam1, DWORD dwUnknownParam2, DWORD dwUnknownParam3, const float fVolumeLevel, DWORD dwUnknowParam4);
void(_stdcall* pChannelStop)(char cUnknownOffset);

void check()
{
	DWORD addrToCall = NULL;
	while (true)
	{
		if (GetKeyState(VK_F3) & 0x8000)
		{
			if (dwBaseSampAddress == NULL)
			{
				dwBaseSampAddress = (DWORD)GetModuleHandle(L"samp.dll");
				pStartRadioPlay = (void(_stdcall *)(const char*, DWORD, DWORD, DWORD, const float, DWORD))(dwBaseSampAddress + dwBaseSampAddressOffsetToPlayFunc);
				pChannelStop = (void(_stdcall *)(char))(dwBaseSampAddress + dwBaseSampAddressOffsetToStopFunc);
			}
			if (GetKeyState(0x35) & 0x8000)
			{
				_asm mov ecx, one//SA-MP checks(compares with 1) [ecx] and if ecx is unreadable then fault and crash
				pChannelStop(1);
			
			}
			else
			{
				_asm mov ecx, one//SA-MP checks(compares with 1) [ecx] and if ecx is unreadable then fault and crash
				pStartRadioPlay(radioStation, 0, 0, 0, 50.0f, 0);
			}
		}
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

