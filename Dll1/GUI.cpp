#include "GUI.h"



HRESULT _stdcall GUI::newEndScene(LPDIRECT3DDEVICE9 pDevice)
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

HRESULT _stdcall GUI::newPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion)
{
	return originalPresent(pDevice, pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT _stdcall GUI::newReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS * pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = originalReset(pDevice, pPresentationParameters);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}

void GUI::init()
{
	DWORD **pPointer = (DWORD**)GTA_SA_D3D9_OFFSET;
	DWORD ppPointer = **pPointer;
	originalEndScene = (oEndScene)setNewPointer(ppPointer + 0xA8, (DWORD)&newEndScene);
	originalPresent = (oPresent)setNewPointer(ppPointer + 0x44, (DWORD)&newPresent);
	originalReset = (oReset)setNewPointer(ppPointer + 0x40, (DWORD)&newReset);
}

void GUI::clean()
{
	if (bInited)
	{
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void GUI::switchShowMenu()
{
	bShow = !bShow;
}

GUI::GUI() = default;


GUI::~GUI() = default;


/*
	Pointers to functions
*/
oEndScene GUI::originalEndScene = nullptr;
oReset GUI::originalReset = nullptr;
oPresent GUI::originalPresent = nullptr;
//
bool GUI::bInited = false;
ImGuiIO GUI::io;
bool GUI::bShow = false;
D3DPRESENT_PARAMETERS GUI::params;