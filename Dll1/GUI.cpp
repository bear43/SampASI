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

void GUI::showCursor(bool boolean)
{
	ShowCursor(boolean);
}

shared_ptr<RECT> GUI::freeCursor()
{
	shared_ptr<RECT> rectangleArea = getConfinedRectangle();
	if (rectangleArea)
	{
		ClipCursor(nullptr);
	}
	return rectangleArea;
}

shared_ptr<RECT> GUI::getConfinedRectangle()
{
	shared_ptr<RECT> rectangleArea = nullptr;
	GetClipCursor(rectangleArea.get());
	return rectangleArea;
}

void GUI::confineCursor(shared_ptr<RECT> rectangle)
{
	if (!rectangle) return;
	ClipCursor(rectangle.get());
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
	static RECT s_confinedRectangle;
	bShow = !bShow;
	ClipCursor(nullptr);
	/*if (bShow)
	{
		s_confinedRectangle = *freeCursor();
	}
	else
	{
		confineCursor(make_shared<RECT>(s_confinedRectangle));
	}*/
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