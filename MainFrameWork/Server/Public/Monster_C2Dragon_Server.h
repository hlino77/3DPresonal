#pragma once

#include "Server_Defines.h"
#include "Monster_Server.h"

BEGIN(Server)

class CSkill_Hiryu_Server;

class CMonster_C2Dragon_Server : public CMonster_Server
{
protected:
	CMonster_C2Dragon_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_C2Dragon_Server(const CMonster_C2Dragon_Server& rhs);
	virtual ~CMonster_C2Dragon_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;
	virtual void	Set_Skill(CGameObject* pObject) override;
public:
	void				Send_MonsterInfo();


	void				Update_NearTarget(_float fTimeDelta);


	void				Spawn();

	void				Shoot_Hiryu();
	

	CSkill_Hiryu_Server* Get_Hiryu() { return m_pHiryu; }
public:
	void				Set_AttackDelay(_float fTimeDelta) { m_fAttackDelay = fTimeDelta; }
	_float				Get_AttackDelay() { return m_fAttackDelay; }

	virtual void		Set_Die();
protected:
	virtual HRESULT		Ready_Components();

private:
	void				Ready_State();


	void				Set_ToDeidara();

	
private:
	_float	m_fSendInfoTime = 0.0f;

	_float	m_fFindTargetTime = 0.0f;


	_float	m_fAttackDelay = 0.0f;

	_float	m_fSpawnTime = 0.0f;


	CSkill_Hiryu_Server* m_pHiryu = nullptr;
public:
	static	CMonster_C2Dragon_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

