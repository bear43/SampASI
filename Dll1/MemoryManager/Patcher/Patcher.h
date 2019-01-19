#pragma once
#include <Windows.h>

/*
	destinationAddress - адрес для установки доступа
	newProtect - какой режим доступа будет?
	bytesCount - на сколько байт распространяется установка
*/
void setAddressProtection(DWORD destinationAddress, DWORD newProtect, unsigned int bytesCount);

/*
	Восстанавливает флаги доступа после последнего вызова setAddressProtection.
	Вызывает setAddressProtection. oldProtect также изменяется ввиду этого.
	destinationAddress - адрес для установки доступа
	bytesCount - на сколько байт распространяется установка
*/
void restoreLastProtection(DWORD destinationAddress, unsigned int bytesCount);

/*
	Вызывающая программа должна чистить память за собой!!!
	sourceAddress - откуда берем байты
	count - количество байт для чтения
*/
char* getBytes(DWORD sourceAddress, unsigned int count);

/*
	destinationAddress - адрес для патча байт
	newBytes - новые байты
	size - количество байт для патча(размер newBytes)
*/
void patchBytes(DWORD destinationAddress, const char newBytes[], unsigned int size);

/*
	destinationAddress - адрес для патча байт
	newValue - Новое значение указателя
*/
void patchPointer(DWORD destinationAddress, DWORD newValue);