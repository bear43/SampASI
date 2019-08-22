#include "CSamp.h"

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

void CSamp::RakPatch()
{
	dwRaknetInfo = (*(DWORD*)
							((*(DWORD*)(dwBaseSampAddress + SAMP_GAME_INFO_STRUCT_OFFSET)) + 0x2C));
	ReceivePointerOriginal = (SAMPReceive)setNewPointer((*(DWORD*)dwRaknetInfo) + 0x20, (DWORD)& Receive);
}

void CSamp::RestartGame()
{
	SAMPRestartGame((void*)(*(DWORD*)(dwBaseSampAddress + SAMP_GAME_INFO_STRUCT_OFFSET)));
}

Packet* CSamp::Receive(DWORD Unk)
{
	Packet* packet = ReceivePointerOriginal((void*)dwRaknetInfo);
	if (packet)
	{
		BitStream bs(packet->data, packet->length, false);
		char packetId;
		bs.Read(packetId);
		char* chars = new char[packet->length - 1];
		bs.Read(chars, packet->length - 1);
		GUI::updatePacketData((unsigned char*)chars, packet->length - 1);
		delete[] chars;
	}
	return packet;
}


restartGame CSamp::SAMPRestartGame = nullptr;
addMessageToChat CSamp::sampAddMessageToChat = nullptr;
DWORD CSamp::dwBaseSampAddress = NULL;
DWORD CSamp::dwOffsetToGameState = 0x0;
DWORD CSamp::dwRaknetInfo = 0x0;
SAMPReceive CSamp::ReceivePointerOriginal = nullptr;
bool CSamp::patched = false;