#pragma once
#include "ServerEvent.h"

BEGIN(Server)

class CLevel_Arena_Server;

class CServerEvent_BattleStart : public CServerEvent
{
public:
	CServerEvent_BattleStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CLevel_Arena_Server* pLevel);
	virtual ~CServerEvent_BattleStart() = default;

public:
	virtual HRESULT Initialize() override;

	virtual void Enter_Event() override;
	virtual void Exit_Event() override;

	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
protected:
	EVENTSTATE eState;
	CLevel_Arena_Server* m_pLevel = nullptr;


	_float m_fDelayTime = 0.0f;
	_bool m_bEnd = false;
public:
	virtual void Free();

};

END