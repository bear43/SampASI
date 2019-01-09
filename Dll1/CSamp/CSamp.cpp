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
		unsigned char* addressToPatch = (unsigned char*)(dwBaseSampAddress + dwOffsetToReconnectDelay);
		/* New bytes AA 00 00 00*/
		DWORD oldProtect;
		VirtualProtect(addressToPatch, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
		addressToPatch[1] = 0xAA;
		addressToPatch[2] = 0x00;
		addressToPatch[3] = 0x00;
		addressToPatch[4] = 0x00;
		VirtualProtect(addressToPatch, 5, oldProtect, &oldProtect);
		CSamp::sendMessage("[Decreased connection delay] Byte patched succsessful!");
		patched = true;
	}
}

/* Send message to chat */
void CSamp::sendMessage(string message)
{
	addToChat(message.c_str(), 0xFF00FF00);
}

DWORD CSamp::dwBaseSampAddress = NULL;
bool CSamp::patched = false;