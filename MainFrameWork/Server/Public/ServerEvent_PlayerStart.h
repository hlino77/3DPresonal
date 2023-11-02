#pragma once
#include "ServerEvent.h"

BEGIN(Server)

class CLevel_Arena_Server;

class CServerEvent_PlayerStart : public CServerEvent
{
public:
	CServerEvent_PlayerStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CServerEvent_PlayerStart() = default;

public:
	virtual HRESULT Initialize() override;

	virtual void Enter_Event() override;
	virtual void Exit_Event() override;

	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
protected:
	EVENTSTATE eState;


public:
	virtual void Free();

};

END