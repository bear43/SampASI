#pragma once
#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_dx9.h"
#include "imgui/examples/imgui_impl_win32.h"
#include <d3d9.h>
#include <Windows.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "Xinput.lib")
#pragma comment(lib, "Xinput9_1_0.lib")

#define GTA_SA_D3D9_OFFSET 0xC97C28

class GUI
{
private:

public:
	GUI();
	~GUI();
};

