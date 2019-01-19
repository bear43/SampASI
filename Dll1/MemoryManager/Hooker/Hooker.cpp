#include "Hooker.h"

/*
	Устанавливает jmp destinationAddress на адрес инструкции, откуда нужен хук.
	Возвращает оригинальные байты
	hookAddress - Адрес для замены байтами прыжка - jmp destionationAddress
	Чистим память!!!
*/
char* setHook(DWORD hookAddress, DWORD destinationAddress)
{
	char* originalBytes = getBytes(hookAddress, 5);
	/*
		jmp destinationAddress
		E9 AA BB CC DD
	*/
	DWORD relativeJumpAddress = hookAddress - destinationAddress + 5;
	const char newByte[] = { 0xE9, relativeJumpAddress >> 24, (relativeJumpAddress >> 16) & 0xFF, (relativeJumpAddress >> 8) & 0xFF, relativeJumpAddress & 0xFF };
	patchBytes(hookAddress, newByte, 5);
	return originalBytes;
}

/*
	Просто возвращает hookAddress + 5
	=)
*/
DWORD getAddressToOriginalCode(DWORD hookAddress)
{
	return hookAddress + 5;
}

DWORD setNewPointer(DWORD address, DWORD newValue)
{
	char* bytes = getBytes(address, 4);
	DWORD oldValue = *((DWORD*)bytes);
	patchPointer(address, newValue);
	free(bytes);
	return oldValue;
}