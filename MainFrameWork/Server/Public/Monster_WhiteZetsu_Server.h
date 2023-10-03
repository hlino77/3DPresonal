#pragma once

#include "Server_Defines.h"
#include "Monster_Server.h"


BEGIN(Server)
class CMonster_WhiteZetsu_Server : public CMonster_Server
{
protected:
	CMonster_WhiteZetsu_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_WhiteZetsu_Server(const CMonster_WhiteZetsu_Server& rhs);
	virtual ~CMonster_WhiteZetsu_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void				Send_MonsterInfo();

protected:
	virtual HRESULT Ready_Components();

private:
	void Ready_State();


private:
	_float	m_fSendInfoTime = 0.0f;

public:
	static	CMonster_WhiteZetsu_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

