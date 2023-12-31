#pragma once
#include "ClientEvent.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUI_BattleStart;

class CClientEvent_BattleStart : public CClientEvent
{
public:
	CClientEvent_BattleStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CClientEvent_BattleStart() = default;

public:
	virtual HRESULT Initialize();

	virtual void Enter_Event();
	virtual void Exit_Event();

	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	virtual HRESULT Render();

protected:
	EVENTSTATE eState;

	CUI_BattleStart* m_pBattleStart = nullptr;


	_float m_fDelayTime = 0.0f;
public:
	virtual void Free();
};

END