#pragma once

#include "Server_Defines.h"
#include "Skill_Server.h"


BEGIN(Server)
class CSkill_FireBall_Server : public CSkill_Server
{
protected:
	CSkill_FireBall_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FireBall_Server(const CSkill_FireBall_Server& rhs);
	virtual ~CSkill_FireBall_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther);

	virtual void	Explosion() override;
	
	virtual HRESULT					Ready_Coliders();

public:
	void				Send_SkillInfo();

	

protected:
	virtual HRESULT Ready_Components();


private:


	_bool	m_bExplosion = false;
public:
	static	CSkill_FireBall_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

