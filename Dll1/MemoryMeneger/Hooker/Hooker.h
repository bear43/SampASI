#ifndef __HOOKER_C_
#define __HOOKER_C_
#include "..\Patcher\Patcher.h"

/*
	Устанавливает jmp (hookAddress-destinationAddress+5) на адрес инструкции, откуда нужен хук.
	Возвращает оригинальные байты
	hookAddress - Адрес для замены байтами прыжка - jmp destionationAddress
	Чистим память!!!
*/
DLL_EI char* setHook(DWORD hookAddress, DWORD destinationAddress);

DLL_EI DWORD getAddressToOriginalCode(DWORD hookAddress);

#endif