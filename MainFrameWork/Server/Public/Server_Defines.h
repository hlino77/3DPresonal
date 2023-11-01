#pragma once

/* 클라이언트에 존재하는 모든 클래스가 인클루드해야할 내용을 담는다. */
#include <process.h>

namespace Server
{
	enum LEVELID { LEVEL_STATIC, LEVEL_READY, LEVEL_LOBBY, LEVEL_ARENA, LEVEL_LOADING, LEVEL_END };


	enum OBJ_TYPE { PLAYER, MONSTER, BOSS, SKILL, TERRAIN, PROP, UI, COLMESH, OBJ_END };

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
		EVENTEND
	};

}

extern atomic<_uint> g_iObjectID;

using namespace Server;

