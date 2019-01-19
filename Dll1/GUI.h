#pragma once
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_dx9.h"
#include "imgui/examples/imgui_impl_win32.h"
#include <d3d9.h>
#include <Windows.h>
#include "MemoryManager/Hooker/Hooker.h"
#include "MemoryManager/Patcher/Patcher.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "Xinput.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

#define GTA_SA_D3D9_OFFSET 0xC97C28

typedef HRESULT(_stdcall* oEndScene)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(_stdcall* oPresent)(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
typedef HRESULT(_stdcall* oReset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);

class GUI
{
private:
	static oEndScene originalEndScene;
	static oPresent originalPresent;
	static oReset originalReset;
	static HRESULT _stdcall newEndScene(LPDIRECT3DDEVICE9 pDevice);
	static HRESULT _stdcall newPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	static HRESULT _stdcall newReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static bool bInited;
	static ImGuiIO io;
	static bool bShow;
	static D3DPRESENT_PARAMETERS params;
public:
	static void init();
	static void clean();
	static void switchShowMenu();
	GUI();
	~GUI();
};

