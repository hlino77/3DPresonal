#pragma once

/* Ŭ���̾�Ʈ�� �����ϴ� ��� Ŭ������ ��Ŭ����ؾ��� ������ ��´�. */
#include <process.h>


namespace Client
{
	enum LEVELID { LEVEL_STATIC, LEVEL_LOGO, LEVEL_LOBBY, LEVEL_ARENA, LEVEL_LOADING, LEVEL_END };

	enum OBJ_TYPE { PLAYER, MONSTER, BOSS, SKILL, TERRAIN, PROP, UI, COLMESH, EFFECT, OBJ_END };

	const unsigned int		g_iWinSizeX = 1280;
	const unsigned int		g_iWinSizeY = 720;

	enum class LAYER_TYPE
	{
		LAYER_COLMESH,
		LAYER_TERRAIN,
		LAYER_SKYBOX,
		LAYER_CAMERA,
		LAYER_BACKGROUND,
		LAYER_PLAYER,
		LAYER_MONSTER,
		LAYER_BOSS,
		LAYER_SKILL,
		LAYER_EFFECT,
		LAYER_UI,
		LAYER_END
	};

	enum class LAYER_COLLIDER
	{
		LAYER_BODY,
		LAYER_ATTACK,
		LAYER_CHILD,
		LAYER_END
	};


	enum class COLLIDER_ATTACK
	{
		MIDDLE,
		LEFT,
		RIGHT,
		SPINBLOWUP,
		SPINBLOWDOWN,
		ATTACK_END
	};

	enum LEVELSTATE { LOADING, INITREADY, INITSTART, INITEND, GAMEPLAY, STATEEND };

	enum class EVENTSTATE { EVENTSTART, READY, EVENT, EVENTEND, SETEND, TYPEEND };

	enum class EVENT
	{
		ARENASTART,
		PLAYERSTART,
		BATTLESTART,
		EVENTEND
	};


}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;
using namespace Client;



#define SERVER_IP		L"25.10.18.222"
#define SERVER_PORT		7777