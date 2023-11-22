#pragma once

#include "Server_Defines.h"
#include "Skill_Server.h"


BEGIN(Server)
class CSkill_Meteor_Server : public CSkill_Server
{
protected:
	CSkill_Meteor_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Meteor_Server(const CSkill_Meteor_Server& rhs);
	virtual ~CSkill_Meteor_Server() = default;

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

	void			Appear();
	
	virtual HRESULT					Ready_Coliders();

public:
	void				Send_SkillInfo();


protected:
	virtual HRESULT Ready_Components();


private:
	_float	m_fCurrTime = 0.0f;
	_float	m_fTimer = 0.0f;
	_float	m_fExplosionTime = 0.2f;

	_float	m_fLookSpeed = 0.0f;

	_bool	m_bExplosion = false;


	Vec3 m_vStartPos;
public:
	static	CSkill_Meteor_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

