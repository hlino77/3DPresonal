#pragma once

/* Ŭ���̾�Ʈ�� �����ϴ� ��� Ŭ������ ��Ŭ����ؾ��� ������ ��´�. */
#include <process.h>

namespace Server
{
	enum LEVELID { LEVEL_STATIC, LEVEL_READY, LEVEL_LOBBY, LEVEL_ARENA, LEVEL_LOADING, LEVEL_END };


	enum OBJ_TYPE { PLAYER, MONSTER, TERRAIN, PROP, UI, OBJ_END };

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

	enum class COLLIDER_ATTACK
	{
		MIDDLE,
		LEFT,
		RIGHT,
		ATTACK_END
	};

	enum LEVELSTATE { LOADING, INITREADY, INITSTART, INITEND, GAMEPLAY, STATEEND };
}

extern atomic<_uint> g_iObjectID;

using namespace Server;

