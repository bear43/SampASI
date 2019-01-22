#include "GUI.h"



HRESULT _stdcall GUI::newEndScene(LPDIRECT3DDEVICE9 pDevice)
{
	if (!bInited)
	{
		ImGui::CreateContext();
		io = &ImGui::GetIO();
		io->MouseDrawCursor = false;
		ImGui::StyleColorsLight();
		ImGui_ImplWin32_Init(FindWindowA(NULL, "GTA:SA:MP"));
		ImGui_ImplDX9_Init(pDevice);
		bInited = true;
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	showCursor(bShow);
	if (bShow)
	{
		SetCursor(LoadCursor(NULL, IDC_ARROW));
		ImGui::Begin("Test");
		{
			static int choosedItem = 0;
			int count = CRadio::getAllInstances().size();
			char **stations = new char*[count];
			for (int i = 0; i < count; i++)
			{
				stations[i] = (char*)CRadio::getAllInstances()[i]->getURL().c_str();
			}
			ImGui::PushItemWidth(ImGui::GetWindowWidth()*0.6f);
			if(ImGui::Combo("Radiostations", &choosedItem, stations, count))
			{
			}
			ImGui::PopItemWidth();
			if (ImGui::Button("Play"))
			{
				CRadio::getAllInstances()[choosedItem]->play();
			}
			ImGui::SameLine();
			ImGui::Spacing();
			if (ImGui::Button("Stop"))
			{
				CRadio::stop();
			}
		}
		ImGui::End();
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
	return result;
}

void GUI::requestUntoggleCamera()
{
	DWORD sampAddress = CSamp::getBaseAddress() + SAMP_STRUCT_1;
	sampAddress = *(DWORD*)sampAddress;
	sampAddress += 0x65;
	setAddressProtection(sampAddress, PAGE_EXECUTE_READWRITE, 4);
	*(DWORD*)sampAddress = 0x0;
	restoreLastProtection(sampAddress, 4);
}

LRESULT WINAPI GUI::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
	return originalWndProc(hWnd, msg, wParam, lParam);
}

void GUI::showCursor(bool boolean)
{
	static bool prevState = false;
	if (prevState == boolean) return;
	sampShowCursor((void*)(CSamp::getBaseAddress() + SAMP_STRUCT_1), boolean ? 3 : 0, !boolean);
	if (boolean)
	{
		sampShowCursor((void*)(CSamp::getBaseAddress() + SAMP_STRUCT_1), 3, 0);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
	else
	{
		sampShowCursor((void*)(CSamp::getBaseAddress() + SAMP_STRUCT_1), 0, 1);
		requestUntoggleCamera();
		SetCursor(NULL);
	}
	prevState = boolean;
}

void GUI::init()
{
	DWORD **pPointer = (DWORD**)GTA_SA_D3D9_OFFSET;
	DWORD ppPointer = **pPointer;
	originalEndScene = (oEndScene)setNewPointer(ppPointer + 0xA8, (DWORD)&newEndScene);
	originalPresent = (oPresent)setNewPointer(ppPointer + 0x44, (DWORD)&newPresent);
	originalReset = (oReset)setNewPointer(ppPointer + 0x40, (DWORD)&newReset);
	sampShowCursor = (funcSampShowCursor)(CSamp::getBaseAddress() + SAMP_TOGGLE_CURSOR_FUNC);
	originalWndProc = (oWndProc)SetWindowLongPtrA(FindWindowA(NULL, "GTA:SA:MP"), GWLP_WNDPROC, (LONG)WndProc);
}

void GUI::clean()
{
	if (bInited)
	{
		DWORD **pPointer = (DWORD**)GTA_SA_D3D9_OFFSET;
		DWORD ppPointer = **pPointer;
		originalEndScene = (oEndScene)setNewPointer(ppPointer + 0xA8, (DWORD)originalEndScene);
		originalPresent = (oPresent)setNewPointer(ppPointer + 0x44, (DWORD)originalPresent);
		originalReset = (oReset)setNewPointer(ppPointer + 0x40, (DWORD)originalReset);
		SetWindowLongA(GetActiveWindow(), GWLP_WNDPROC, (LONG)originalWndProc);
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

	}
}

void GUI::switchShowMenu()
{
	bShow = !bShow;
	showCursor(bShow);
}

GUI::GUI() = default;


GUI::~GUI() = default;


/*
	Pointers to functions
*/
oEndScene GUI::originalEndScene = nullptr;
oReset GUI::originalReset = nullptr;
oPresent GUI::originalPresent = nullptr;
funcSampShowCursor GUI::sampShowCursor = nullptr;
oWndProc GUI::originalWndProc = nullptr;
//
bool GUI::bInited = false;
ImGuiIO *GUI::io = nullptr;
bool GUI::bShow = false;