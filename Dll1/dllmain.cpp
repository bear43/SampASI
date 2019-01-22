// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "File\FileOutput.h"
#include "File\FileInput.h"
#include "CRadio\CRadio.h"
#include "GUI.h"

constexpr auto DELAY_TIME = 30;

void check()
{
	CSamp::setBaseAddres();
	if (!CSamp::checkBaseAddress()) return;//Exit if no samp.dll
	while (!CSamp::isGameReady())//Very bad. Bydlo code, GOVNO
	{
		Sleep(150);
	}
	CSamp::patchConnectDelayTimer();//Patches delay
	CRadio::init();
	CRadio::loadAllInstances();
	GUI::init();
	bool alreadyPressed1 = false;
	while (true)
	{
		if (!CSamp::isInPause())
		{
			if (!alreadyPressed1 && GetKeyState(VK_F3) & 0x8000)
			{
				GUI::switchShowMenu();
			}
			if (GetKeyState(VK_F12))
			{
				CSamp::sendMessage("Exiting from plugin");
				break;
			}
			alreadyPressed1 = GetKeyState(VK_F3) & 0x8000;
		}
		Sleep(DELAY_TIME);
	}
	CRadio::stop();
	GUI::clean();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, LPTHREAD_START_ROUTINE(check), 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
		if(!CRadio::getAllInstances().empty())
			CRadio::saveAllInstances();
        break;
    }
    return TRUE;
}

