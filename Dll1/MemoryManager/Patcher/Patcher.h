#ifndef __PATCHER_C__
#define __PATCHER_C__
#include <Windows.h>
#pragma comment(lib, "MemoryManager.lib")

#ifdef __DLL__
#define DLL_EI __declspec(dllexport)
#else
#define DLL_EI __declspec(dllimport)
#endif

/*
	destinationAddress - адрес дл€ установки доступа
	newProtect - какой режим доступа будет?
	bytesCount - на сколько байт распростран€етс€ установка
*/
DLL_EI void setAddressProtection(DWORD destinationAddress, DWORD newProtect, unsigned int bytesCount);

/*
	¬осстанавливает флаги доступа после последнего вызова setAddressProtection.
	¬ызывает setAddressProtection. oldProtect также измен€етс€ ввиду этого.
	destinationAddress - адрес дл€ установки доступа
	bytesCount - на сколько байт распростран€етс€ установка
*/
DLL_EI void restoreLastProtection(DWORD destinationAddress, unsigned int bytesCount);

/*
	¬ызывающа€ программа должна чистить пам€ть за собой!!!
	sourceAddress - откуда берем байты
	count - количество байт дл€ чтени€
*/
DLL_EI char* getBytes(DWORD sourceAddress, unsigned int count);

/*
	destinationAddress - адрес дл€ патча байт
	newBytes - новые байты
	size - количество байт дл€ патча(размер newBytes)
*/
DLL_EI void patchBytes(DWORD destinationAddress, const char newBytes[], unsigned int size);

#endif