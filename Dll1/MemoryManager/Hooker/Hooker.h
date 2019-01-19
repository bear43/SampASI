#pragma once
#include "..\Patcher\Patcher.h"

/*
	Устанавливает jmp (hookAddress-destinationAddress+5) на адрес инструкции, откуда нужен хук.
	Возвращает оригинальные байты
	hookAddress - Адрес для замены байтами прыжка - jmp destionationAddress
	Чистим память!!!
*/
char* setHook(DWORD hookAddress, DWORD destinationAddress);

DWORD getAddressToOriginalCode(DWORD hookAddress);

DWORD setNewPointer(DWORD address, DWORD newValue);