#pragma once
#include <process.h>
#include <iostream>
namespace Client
{
	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	//LEVEL_STATIC ��� ������ ���������� ���� ��, �÷��̾� ó�� ������ �ٲ� �״�� �����Ǿ�� �ϴ� �κ�
	enum LEVELID { LEVEL_STATIC, LEVEL_LOADING, LEVEL_MENU, LEVEL_STAGE1, LEVEL_STAGE2, LEVEL_BOSS, LEVEL_END };
    enum GAMEOBJ_TYPE{ACTOR,SKILL,MAP,ITEM,NPC,CAMERA, GAMEOBJ_TYPE_END};
    enum UIID { UIID_Menu, UIID_Cursor, UIID_Loading, UIID_PlayerHP, UIID_PlayerState, UIID_PlayerWeaPon, UIID_PlayerWeaPon_Aim,
		        UIID_PlayerShooting, UIID_Monster, UIID_InteractiveUI, UIID_BossHP,UIID_Fade, UIID_END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

#include <random>

using namespace std;
using namespace Client;
