#include "CSamp.h"

void CSamp::FloodFunction()
{
	while (isFlood)
	{
		SendNewsDialog();
		Sleep(SleepMS);
	}
}

void CSamp::SetFloodTiming(unsigned int ms)
{
	SleepMS = ms;
}

bool CSamp::GetFloodState()
{
	return isFlood;
}

void CSamp::KillNewsFlood()
{
	if(isFlood)
		isFlood = false;
}

void CSamp::StartNewsFlood()
{
	if (!isFlood)
	{
		isFlood = true;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)FloodFunction, 0, 0, 0);
	}
}

bool CSamp::SendNewsDialog()
{
	return SendDialogResponse(22, 1, 2, (char*)"[2] Отправить");
}

bool CSamp::SendDialogResponse(unsigned short DialogID, char ButtonID, unsigned short ListBoxItemID, char* InputResponse)
{
	unsigned short respLen = strlen(InputResponse);
	BitStream bsSend;
	bsSend.Write(DialogID);
	bsSend.Write(ButtonID);
	bsSend.Write(ListBoxItemID);
	bsSend.Write(respLen);
	bsSend.Write(InputResponse, respLen);
	return SendRPC(bsSend, RPC_DialogResponse);
}

bool CSamp::SendRPC(BitStream& bs, RPCEnumeration id)
{
	int iID = id;
	return RakNet->RPC(&iID, &bs, SYSTEM_PRIORITY, UNRELIABLE, 0, 0);
}

bool CSamp::HookedRPC(void* self, int* uniqueID, BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, bool shiftTimestamp)
{
	if (bitStream && *uniqueID == RPC_DialogResponse)
	{
		unsigned short DialogID;
		char ButtonID;
		unsigned short ListBoxItemID;
		unsigned char len;
		char* InputResponse;
		bitStream->Read(DialogID);
		bitStream->Read(ButtonID);
		bitStream->Read(ListBoxItemID);
		bitStream->Read(len);
		InputResponse = new char[len + 1];
		memset(InputResponse, '\0', len + 1);
		bitStream->Read(InputResponse, len);
		char buffer[512];
		sprintf_s(buffer, "ID: %i | Button: %i | List: %i | Response: %s\0", 
			DialogID, ButtonID, ListBoxItemID, InputResponse);
		GUI::UpdateOutputDialogData((unsigned char*)buffer, 512);
		delete[] InputResponse;
	}
	return oRPC(self, uniqueID, bitStream, priority, reliability, orderingChannel, shiftTimestamp);
}

void CSamp::addToChat(const char message[], DWORD color)
{
	if (sampAddMessageToChat != nullptr) sampAddMessageToChat((void*)(*((DWORD*)(dwBaseSampAddress + dwOffsetToChatInfo))), 8, message, 0, color, 0);
}

void CSamp::SetGameState(DWORD state)
{
	if (dwOffsetToGameState == 0x0)
		if (dwBaseSampAddress == 0x0) return;
		else
			dwOffsetToGameState = *(DWORD*)(dwBaseSampAddress + SAMP_GAME_INFO_STRUCT_OFFSET) + 0x3CD;
	setAddressProtection(dwOffsetToGameState, PAGE_READWRITE, 4);
	*(DWORD*)(dwOffsetToGameState) = state;
	restoreLastProtection(dwOffsetToGameState, 4);
}

void CSamp::setBaseAddres()
{
	dwBaseSampAddress = (DWORD)GetModuleHandle(L"samp.dll");
	if (dwBaseSampAddress != 0x0)
	{
		sampAddMessageToChat = (addMessageToChat)(dwBaseSampAddress + dwOffsetToChatAddFunc);
		SAMPRestartGame = (restartGame)(dwBaseSampAddress + dwOffsetToReconnectFunction);
	}
}

/* Checks base samp.dll address */
bool CSamp::checkBaseAddress()
{
	return dwBaseSampAddress;
}

/* Checks game state. If pointer on the struct of SAMP is null then game is not started yet */
bool CSamp::isGameReady()
{
	if (!checkBaseAddress()) return false;
	bool *SAMPstruct = (bool*)(dwBaseSampAddress + SAMP_GAME_INFO_STRUCT_OFFSET);
	return *SAMPstruct;
}

DWORD CSamp::getBaseAddress()
{
	return dwBaseSampAddress;
}

/* Decrease reconnect timer */
void CSamp::patchConnectDelayTimer()
{
	if (!patched)
	{
		DWORD pathAddress = dwBaseSampAddress + dwOffsetToReconnectDelay;
		unsigned char *bytes = (unsigned char*)getBytes(pathAddress, 5);
		bytes[1] = 0xAA;
		bytes[2] = 0x00;
		bytes[3] = 0x00;
		bytes[4] = 0x00;
		patchBytes(pathAddress, (const char*)bytes, 5);
		free(bytes);
		CSamp::sendMessage("[Decreased connection delay] Byte patched succsessful!");
		patched = true;
	}
}

/* Send message to chat */
void CSamp::sendMessage(string message, DWORD color)
{
	addToChat(message.c_str(), color);
}

bool CSamp::isInPause()
{
	static char* gameStatus = (char*)((DWORD)GetModuleHandleA("gta_sa.exe") + dwOffsetToPauseMenuStatus);
	return *gameStatus == 1;
}

void CSamp::OnShowDialog(RPCParameters* params)
{
	BitStream bs(params->input, (params->numberOfBitsOfData / 8)+1, false);
	stInputDialog dialog(bs);
	char buffer[512];
	sprintf_s(buffer, "ID: %i | Type: %i | Title: %s\0", dialog.ID, dialog.DialogType, dialog.Title);
	GUI::UpdateInputDialogData((unsigned char*)buffer, 512);
	oOnShowDialog(params);
}

void CSamp::RakPatch()
{
	dwRaknetInfo = (*(DWORD*)
							((*(DWORD*)(dwBaseSampAddress + SAMP_GAME_INFO_STRUCT_OFFSET)) + 0x2C));
	RakNet = (RakClientInterface*)dwRaknetInfo;
	int tmp = RPC_ShowDialog;
	RakNet->RegisterAsRemoteProcedureCall(&tmp, OnShowDialog);
	oOnShowDialog = (pOnShowDialog)(dwBaseSampAddress + dwOffsetToSampOnShowDialog);
	ReceivePointerOriginal = (SAMPReceive)setNewPointer((*(DWORD*)dwRaknetInfo) + 0x20, (DWORD)& Receive);
	oRPC = (SAMPRPC)setNewPointer((*(DWORD*)dwRaknetInfo) + 0x64, (DWORD)&HookedRPC);
}

void CSamp::RestartGame()
{
	SAMPRestartGame((void*)(*(DWORD*)(dwBaseSampAddress + SAMP_GAME_INFO_STRUCT_OFFSET)));
}

Packet* __thiscall CSamp::Receive(void* self)
{
	return ReceivePointerOriginal(self);
}


restartGame CSamp::SAMPRestartGame = nullptr;
addMessageToChat CSamp::sampAddMessageToChat = nullptr;
DWORD CSamp::dwBaseSampAddress = NULL;
DWORD CSamp::dwOffsetToGameState = 0x0;
DWORD CSamp::dwRaknetInfo = 0x0;
pOnShowDialog CSamp::oOnShowDialog = nullptr;
SAMPReceive CSamp::ReceivePointerOriginal = nullptr;
RakClientInterface* CSamp::RakNet = nullptr;
SAMPRPC CSamp::oRPC = nullptr;
bool CSamp::patched = false;
bool CSamp::isFlood = false;
unsigned int CSamp::SleepMS = 500;