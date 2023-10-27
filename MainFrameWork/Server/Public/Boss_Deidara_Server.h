#pragma once

#include "Server_Defines.h"
#include "Boss_Server.h"


BEGIN(Server)
class CMonster_C2Dragon_Server;

class CBoss_Deidara_Server : public CBoss_Server
{
protected:
	CBoss_Deidara_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Deidara_Server(const CBoss_Deidara_Server& rhs);
	virtual ~CBoss_Deidara_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther);

public:
	void				Send_BossInfo();


	void				Update_NearTarget(_float fTimeDelta);

	void				Attack_Random();

	void				Set_C2Dragon(CMonster_C2Dragon_Server* pDragon) { m_pC2Dragon = pDragon; }
	void				Spawn_C2Dragon();

protected:
	virtual HRESULT		Ready_Components();

private:
	void				Ready_State();


private:
	_float	m_fSendInfoTime = 0.0f;

	_float	m_fFindTargetTime = 0.0f;

	CMonster_C2Dragon_Server* m_pC2Dragon = nullptr;

public:
	static	CBoss_Deidara_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

