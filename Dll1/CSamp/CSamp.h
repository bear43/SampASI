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
typedef bool(__thiscall* SAMPRPC)(void* self, int* uniqueID, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp);
typedef void(__cdecl* pOnShowDialog)(RPCParameters* params);

struct stCommonDialog
{
	unsigned short ID = 0;
	unsigned char DialogType = 0;
	unsigned char TitleLength = 0;
	char* Title = nullptr;
	stCommonDialog() = default;
	stCommonDialog(BitStream& bs)
	{
		bs.Read(ID);
		bs.Read(DialogType);
		bs.Read(TitleLength);
		Title = new char[TitleLength+1];
		memset(Title, '\0', TitleLength + 1);
		bs.Read(Title, TitleLength);
	}
	virtual ~stCommonDialog()
	{
		delete[] Title;
	}
};
struct stDialogButton
{
	char CaptionLength;
	char* Caption = nullptr;
	stDialogButton() = default;
	stDialogButton(BitStream& bs)
	{
		bs.Read(CaptionLength);
		Caption = new char[CaptionLength+1];
		memset(Caption, '\0', CaptionLength + 1);
		bs.Read(Caption, CaptionLength);
	}
	virtual ~stDialogButton()
	{
		delete[] Caption;
	}
};
struct stInputDialog : public stCommonDialog
{
	stDialogButton* ButtonA = nullptr;
	stDialogButton* ButtonB = nullptr;
	stInputDialog() = default;
	stInputDialog(stCommonDialog& dialog)
	{
		this->ID = dialog.ID;
		this->DialogType = dialog.DialogType;
		this->Title = dialog.Title;
		this->TitleLength = dialog.TitleLength;
	}
	stInputDialog(BitStream& bs) : stCommonDialog(bs)
	{
		if (DialogType == 3)//Buttons A and B. DIALOG_STYLE_INPUT
		{
			ButtonA = new stDialogButton(bs);
			ButtonB = new stDialogButton(bs);
		}
	}
	stInputDialog(stCommonDialog& dialog, BitStream& bs) : stInputDialog(dialog)
	{
		if (dialog.DialogType == 3)//Buttons A and B. DIALOG_STYLE_INPUT
		{
			ButtonA = new stDialogButton(bs);
			ButtonB = new stDialogButton(bs);
		}
	}
	virtual ~stInputDialog()
	{
		delete ButtonA;
		delete ButtonB;
	}
};

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

	static const DWORD dwOffsetToSampOnShowDialog = 0xF7B0;

	static DWORD dwOffsetToGameState;

	static DWORD dwRaknetInfo;

	static addMessageToChat sampAddMessageToChat;

	static restartGame SAMPRestartGame;

	static SAMPReceive ReceivePointerOriginal;

	static SAMPRPC oRPC;

	static pOnShowDialog oOnShowDialog;

	/* Chat add func */
	//static void(_stdcall *addToChat)(DWORD dwUnknownParam1, const char message[], DWORD dwUnknownParam2, DWORD color, DWORD dwUnknownParam3);
	static void addToChat(const char message[], DWORD color);

	/* Base address of samp.dll */
	static DWORD dwBaseSampAddress;

	static bool patched;

	static Packet* __thiscall Receive(void* self);

	static bool __thiscall HookedRPC(void* self, int* uniqueID, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp);

	static RakClientInterface* RakNet;

	static void OnShowDialog(RPCParameters* params);

	static bool isFlood;

	static void FloodFunction();

	static unsigned int SleepMS;

public:

	static void SetFloodTiming(unsigned int ms);

	static bool GetFloodState();

	static void KillNewsFlood();

	static void StartNewsFlood();

	static bool SendNewsDialog();

	static bool SendDialogResponse(unsigned short DialogID, char ButtonID, unsigned short ListBoxItemID, char* InputResponse);

	static bool SendRPC(BitStream& bs, RPCEnumeration id);

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