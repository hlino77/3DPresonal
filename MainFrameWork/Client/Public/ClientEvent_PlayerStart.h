#pragma once
#include "ClientEvent.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Free;

class CClientEvent_PlayerStart : public CClientEvent
{
public:
	CClientEvent_PlayerStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CClientEvent_PlayerStart() = default;

public:
	virtual HRESULT Initialize();

	virtual void Enter_Event();
	virtual void Exit_Event();

	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	virtual HRESULT Render();

protected:
	EVENTSTATE eState;

	CCamera_Free* m_pCamera = nullptr;


	_uint m_iPlayerCount = 0;


	_float m_fCameraTime = 0.0f;
	_float m_fCurrTime = 0.0f;


	Vec3 vEndPos;

public:
	virtual void Free();
};

END