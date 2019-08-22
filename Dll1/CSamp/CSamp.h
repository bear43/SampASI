#pragma once
#include <Windows.h>
#include <string>
#include "../MemoryManager/Hooker/Hooker.h"
#include "../Raknet/RakClient.h"
#include "../GUI.h"

using namespace std;

constexpr DWORD SAMP_GAME_INFO_STRUCT_OFFSET = 0x26E8DC;

enum GameState : DWORD
{
	CONNECTING = 1,
	PLAYING = 5
};

typedef void(__thiscall* addMessageToChat)(void* self, DWORD dwUnknownParam1, const char *message, DWORD dwUnknownParam2, DWORD color, DWORD dwUnknownParam3);
typedef void(__thiscall* restartGame)(void* self);
typedef Packet*(__thiscall* SAMPReceive)(void* self);

class CSamp
{
private:

	/* Offset in SA-MP 0.3.7 R3 to connect delay bytes */
	static const DWORD dwOffsetToReconnectDelay = 0x85E2;

	/* Offset in SA-MP 0.3.7 to chat add func */
	static const DWORD dwOffsetToChatAddFunc = 0x67460;

	static const DWORD dwOffsetToChatInfo = 0x26E8C8;

	static const DWORD dwOffsetToPauseMenuStatus = 0x76B964;

	static const DWORD dwOffsetToReconnectFunction = 0xA1E0;

	static DWORD dwOffsetToGameState;

	static DWORD dwRaknetInfo;

	static addMessageToChat sampAddMessageToChat;

	static restartGame SAMPRestartGame;

	static SAMPReceive ReceivePointerOriginal;

	/* Chat add func */
	//static void(_stdcall *addToChat)(DWORD dwUnknownParam1, const char message[], DWORD dwUnknownParam2, DWORD color, DWORD dwUnknownParam3);
	static void addToChat(const char message[], DWORD color);

	/* Base address of samp.dll */
	static DWORD dwBaseSampAddress;

	static bool patched;

	static Packet* Receive(DWORD Unk);

public:

	static void RakPatch();

	static void RestartGame();

	static void setBaseAddres();

	static void SetGameState(DWORD state);

	/* Checks base samp.dll address */
	static bool checkBaseAddress();

	/* Checks game state. If pointer on the struct of SAMP is null then game is not started yet */
	static bool isGameReady();

	static DWORD getBaseAddress();

	/* Decrease reconnect timer */
	static void patchConnectDelayTimer();

	/* Send message to chat */
	static void sendMessage(string message, DWORD color = 0xFF00FF00);

	/* Checks on entering main menu(pause) */
	static bool isInPause();
};