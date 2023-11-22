#pragma once
#include "ClientEvent.h"
#include "Client_Defines.h"

BEGIN(Client)

class CCamera_Free;
class CBoss_Madara;

class CClientEvent_MadaraMeteor : public CClientEvent
{
public:
	CClientEvent_MadaraMeteor(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CClientEvent_MadaraMeteor() = default;

public:
	virtual HRESULT Initialize();

	virtual void Enter_Event();
	virtual void Exit_Event();

	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);

	virtual HRESULT Render();


private:
	void		Player_Active(_bool bActive);

	

private:
	EVENTSTATE eState;



	CCamera_Free* m_pCamera = nullptr;

	CBoss_Madara* m_pMadara = nullptr;


	Vec3 m_vOffset;
	Vec3 m_vTargetOffset;
	Vec3 m_vTargetPos;
	Vec3 m_vMadaraPos;


	_bool m_bStart = false;
	_float m_fDelayTime = 0.0f;
public:
	virtual void Free();
};

END