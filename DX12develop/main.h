#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <assert.h>



#pragma warning(push)
#pragma warning(disable:4005)

#include <d3d12.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#pragma warning(pop)



#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d3d12.lib")



#define SCREEN_WIDTH	(960)
#define SCREEN_HEIGHT	(540)


HWND GetWindow();