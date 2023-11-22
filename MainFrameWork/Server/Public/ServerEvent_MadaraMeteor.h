#pragma once
#include "ServerEvent.h"

BEGIN(Server)

class CLevel_Arena_Server;
class CBoss_Madara_Server;

class CServerEvent_MadaraMeteor : public CServerEvent
{
public:
	CServerEvent_MadaraMeteor(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CServerEvent_MadaraMeteor() = default;

public:
	virtual HRESULT Initialize() override;

	virtual void Enter_Event() override;
	virtual void Exit_Event() override;

	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;




private:
	EVENTSTATE eState;

	CBoss_Madara_Server* m_pMadara = nullptr;
	_int m_iAnimIndex = 0;

	Vec3 m_vMadaraPos;

	_float m_fDelayTime;
	_bool m_bSkill = false;

public:
	virtual void Free();

};

END