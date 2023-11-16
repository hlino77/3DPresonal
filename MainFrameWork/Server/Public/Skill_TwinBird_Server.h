#pragma once

#include "Server_Defines.h"
#include "Skill_Server.h"


BEGIN(Server)
class CSkill_TwinBird_Server : public CSkill_Server
{
protected:
	CSkill_TwinBird_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_TwinBird_Server(const CSkill_TwinBird_Server& rhs);
	virtual ~CSkill_TwinBird_Server() = default;

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

	void				Shoot_TwinBird(CGameObject* pTargetObject, Vec3 vPos, Vec3 vLook);

protected:
	virtual HRESULT Ready_Components();


private:
	_float	m_fCurrTime = 0.0f;
	_float	m_fTimer = 0.0f;
	_float	m_fExplosionTime = 0.2f;

	_float	m_fLookSpeed = 0.0f;

	_bool	m_bExplosion = false;

	std::future<HRESULT>			m_PlayAnimation;
public:
	static	CSkill_TwinBird_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

