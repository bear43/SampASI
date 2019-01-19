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

class GUI
{
private:
	static oEndScene originalEndScene;
	static HRESULT _stdcall newEndScene(LPDIRECT3DDEVICE9 pDevice);
	static bool bInited;
	static IDirect3DDevice9 *pDevice;
	static ImGuiIO io;
	static bool bShow;
public:
	static void init();
	static void clean();
	static void switchShowMenu();
	GUI();
	~GUI();
};

