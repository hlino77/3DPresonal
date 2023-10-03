#pragma once

/* 클라이언트에 존재하는 모든 클래스가 인클루드해야할 내용을 담는다. */
#include <process.h>


namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_LOADING, LEVEL_END };

	enum OBJ_TYPE { PLAYER, MONSTER, PROP, OBJ_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum class LAYER_TYPE
	{
		LAYER_TERRAIN,
		LAYER_SKYBOX,
		LAYER_CAMERA,
		LAYER_BACKGROUND,
		LAYER_PLAYER,
		LAYER_MONSTER,
		LAYER_PROJECTILE,
		LAYER_EFFECT,
		LAYER_UI,
		LAYER_END
	};

	enum class LAYER_COLLIDER
	{
		LAYER_BODY,
		LAYER_ATTACK,
		LAYER_END
	};


	enum LEVELSTATE { LOADING, INITREADY, INITSTART, INITEND, STATEEND };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;
using namespace Client;



#define SERVER_IP		L"192.168.200.155"
#define SERVER_PORT		7777