#pragma once
#include <process.h>
#include <iostream>
namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	//LEVEL_STATIC 모든 레벨에 공통적으로 쓰는 것, 플레이어 처럼 레벨이 바뀌어도 그대로 유지되어야 하는 부분
	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_MENU, LEVEL_STAGE1, LEVEL_END };
	enum LOADINGID { GORGE, LOADINGID_END};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;
