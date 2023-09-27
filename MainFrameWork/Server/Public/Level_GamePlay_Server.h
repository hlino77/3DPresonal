#pragma once

#include "Server_Defines.h"
#include "Level.h"

BEGIN(Server)

class CLevel_GamePlay_Server final : public CLevel
{
private:
	CLevel_GamePlay_Server();
	virtual ~CLevel_GamePlay_Server() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

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
public:
	static class CLevel_GamePlay_Server* Create();
	virtual void Free() override;


private:
	_float m_fBroadcastTime = 0.0f;
};

END