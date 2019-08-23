#pragma once
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_dx9.h"
#include "imgui/examples/imgui_impl_win32.h"
#include <d3d9.h>
#include <dinput.h>
#include <Windows.h>
#include <memory>
#include <vector>
#include "MemoryManager/Hooker/Hooker.h"
#include "MemoryManager/Patcher/Patcher.h"
#include "CSamp/CSamp.h"
#include "CRadio/CRadio.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

#define GTA_SA_D3D9_OFFSET 0xC97C28
#define SAMP_TOGGLE_CURSOR_FUNC 0x9FFE0
#define SAMP_STRUCT_1 0x26E8F4//For toggle cursor. SAMP+MISC_INFO
//Strange offset samp.dll+60C31
//samp.dll+613B1


using namespace std;

typedef HRESULT(_stdcall* oEndScene)(LPDIRECT3DDEVICE9 pDevice);
typedef HRESULT(_stdcall* oPresent)(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
typedef HRESULT(_stdcall* oReset)(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
typedef LRESULT(_stdcall* oWndProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef void(__thiscall* funcSampShowCursor)(void* self, DWORD playerControll, DWORD cursor);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class GUI
{
private:
	static oEndScene originalEndScene;
	static oPresent originalPresent;
	static oReset originalReset;
	static HRESULT _stdcall newEndScene(LPDIRECT3DDEVICE9 pDevice);
	static HRESULT _stdcall newPresent(LPDIRECT3DDEVICE9 pDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
	static HRESULT _stdcall newReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters);
	static funcSampShowCursor sampShowCursor;
	static void requestUntoggleCamera();
	static bool bInited;
	static ImGuiIO *io;
	static bool bShow;
	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static oWndProc originalWndProc;
	static vector<char> InData;
	static vector<char> OutData;
public:
	static void showCursor(bool boolean);
	static void init();
	static void clean();
	static void switchShowMenu();
	static void UpdateInputDialogData(unsigned char* data, int length);
	static void UpdateOutputDialogData(unsigned char* data, int length);
	GUI();
	~GUI();
};

