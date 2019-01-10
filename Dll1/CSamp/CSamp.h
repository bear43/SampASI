#pragma once
#include <Windows.h>
#include <string>
#include "..\MemoryManager\Hook.h"

using namespace std;

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
	static void addToChat(const char message[], DWORD color);

	/* Base address of samp.dll */
	static DWORD dwBaseSampAddress;

	static bool patched;

public:

	static void setBaseAddres();

	/* Checks base samp.dll address */
	static bool checkBaseAddress();

	/* Checks game state. If pointer on the struct of SAMP is null then game is not started yet */
	static bool isGameReady();

	static DWORD getBaseAddress();

	/* Decrease reconnect timer */
	static void patchConnectDelayTimer();

	/* Send message to chat */
	static void sendMessage(string message, DWORD color = 0xFF00FF00);
};