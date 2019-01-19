// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "File\FileOutput.h"
#include "File\FileInput.h"
#include "CRadio\CRadio.h"
#include "GUI.h"

constexpr auto DELAY_TIME = 30;

IDirect3DDevice9 *pDevice = nullptr;
bool bInited = false;
ImGuiIO io;
char* originalBytes = nullptr;
bool bShow = false;

typedef HRESULT(_stdcall* oEndScene)(LPDIRECT3DDEVICE9 pDevice);

oEndScene originalEndScene = nullptr;

void patchPointer(DWORD destinationAddress, DWORD newValue)
{
	const char arr[] = { newValue & 0xFF, (newValue >> 8) & 0xFF, (newValue >> 16) & 0xFF, newValue >> 24 };
	patchBytes(destinationAddress, arr, 4);
}

HRESULT _stdcall imguil(LPDIRECT3DDEVICE9 pDevice)
{
	if (!bInited)
	{
		ImGui::CreateContext();
		io = ImGui::GetIO();
		ImGui::StyleColorsClassic();
		ImGui_ImplWin32_Init(FindWindowA(NULL, "GTA:SA:MP"));
		ImGui_ImplDX9_Init(pDevice);
		bInited = true;
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (bShow)
	{
		
		ImGui::ShowDemoWindow();
	}
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	return originalEndScene(pDevice);

}

void hook()
{
	DWORD **pPointer = (DWORD**)GTA_SA_D3D9_OFFSET;
	DWORD ppPointer = **pPointer;
	originalEndScene = (oEndScene)(*(DWORD*)(ppPointer + 0xA8));
	patchPointer(ppPointer + 0xA8, (DWORD)&imguil);
}

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
	hook();
	while (true)
	{
		if (GetKeyState(VK_F3) & 0x8000)
		{
			if (GetKeyState(0x35) & 0x8000)
			{
				CRadio::stop();
				bShow = !bShow;
			}
			else
			{
				CRadio::getAllInstances()[0]->play();
			}
		}
		if (GetKeyState(VK_F12))
		{
			CSamp::sendMessage("Exiting from plugin");
			break;
		}
		Sleep(DELAY_TIME);
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

