#include "CSamp.h"

void CSamp::addToChat(const char message[], DWORD color)
{
	DWORD funcAddress = dwBaseSampAddress + dwOffsetToChatAddFunc;
	DWORD SAMP_CHAT_POINTER = *((DWORD*)(dwBaseSampAddress + dwOffsetToChatInfo));
	if (funcAddress != dwOffsetToChatAddFunc)
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

void CSamp::setBaseAddres()
{
	dwBaseSampAddress = (DWORD)GetModuleHandle(L"samp.dll");
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
	bool *SAMPstruct = (bool*)(dwBaseSampAddress + dwOffsetToSAMPINFO);
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

DWORD CSamp::dwBaseSampAddress = NULL;
bool CSamp::patched = false;