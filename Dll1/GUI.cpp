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

void GUI::init()
{
	DWORD **pPointer = (DWORD**)GTA_SA_D3D9_OFFSET;
	DWORD ppPointer = **pPointer;
	originalEndScene = (oEndScene)setNewPointer(ppPointer + 0xA8, (DWORD)&newEndScene);
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

oEndScene GUI::originalEndScene = nullptr;
IDirect3DDevice9 *GUI::pDevice;
bool GUI::bInited = false;
ImGuiIO GUI::io;
bool GUI::bShow = false;
