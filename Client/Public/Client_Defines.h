#pragma once
#include <process.h>
#include <iostream>
namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	//LEVEL_STATIC ��� ������ ���������� ���� ��, �÷��̾� ó�� ������ �ٲ� �״�� �����Ǿ�� �ϴ� �κ�
	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_MENU, LEVEL_STAGE1, LEVEL_END };
	enum LOADINGID { GORGE, LOADINGID_END};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

using namespace std;
using namespace Client;
