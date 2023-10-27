#pragma once

#include "Server_Defines.h"
#include "Level.h"
#include <thread>

BEGIN(Server)

class CLevel_Arena_Server final : public CLevel
{
private:
	CLevel_Arena_Server();
	virtual ~CLevel_Arena_Server() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

	virtual HRESULT Exit() override;
private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Player(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_BackGround(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Monster(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_UI(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Effect(const LAYER_TYPE eLayerType);


private:
	HRESULT			Broadcast_Character();
	void			Broadcast_LevelState(LEVELSTATE eState);
	void			Wait_ClientLevelState(LEVELSTATE eState);
	HRESULT			Broadcast_PlayerInfo();
	HRESULT			Broadcast_Monster(const wstring& szName, Vec3 vPos);
	HRESULT			Broadcast_Boss(const wstring& szName, Vec3 vPos);



public:
	static class CLevel_Arena_Server* Create();
	virtual void Free() override;


private:
	Vec3			Find_MonsterSpawnPos();

	void			Check_Monster();
	void			Spawn_Monster();




	void			Set_CheckGruop();
	void			Start_Collision();
	void			End_Collision();
private:
	_float m_fBroadcastTime = 0.0f;
	_uint  m_iMonsterCount = 0;

	_uint	m_iMaxMonster = 0;

	_bool	m_bBoss = false;


private:
	
	thread* m_pCollisionThread = nullptr;
};

END