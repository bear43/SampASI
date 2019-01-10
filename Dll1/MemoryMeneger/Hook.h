#pragma once
extern "C"
{
#include "Hooker\Hooker.h"
}

/* Создает хук */
class Hook
{
private:
	char *originalBytes = nullptr;

	DWORD hookAddress;

	DWORD userFunctionAddress;

	bool hooked = false;

	void releaseOriginalBytes();

public:
	DLL_EI Hook(DWORD hookAddress, DWORD userFunctionAddress);
	
	DLL_EI Hook(DWORD hookAddress, void* userFunctionAddress);

	DLL_EI void set();

	DLL_EI void unset();
	
	DLL_EI ~Hook();
};