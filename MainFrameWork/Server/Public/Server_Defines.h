#pragma once

/* Ŭ���̾�Ʈ�� �����ϴ� ��� Ŭ������ ��Ŭ����ؾ��� ������ ��´�. */
#include <process.h>

namespace Server
{
	enum LEVELID { LEVEL_STATIC, LEVEL_READY, LEVEL_GAMEPLAY, LEVEL_LOADING, LEVEL_END };


	enum OBJ_TYPE { PLAYER, MONSTER, PROP, OBJ_END };

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

	enum LEVELSTATE { LOADING, INITREADY, INITSTART, INITEND, STATEEND };
}

extern atomic<_uint> g_iObjectID;

using namespace Server;

