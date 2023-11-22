#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)
class CFireBall;
class CFireBallPlane;
class CFireBallTrail;


class CSkill_FireBall : public CSkill
{
protected:
	CSkill_FireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_FireBall(const CSkill_FireBall& rhs);
	virtual ~CSkill_FireBall() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther);


	virtual void				Explosion() override;

	virtual HRESULT					Ready_Coliders();

public:
	void				Send_SkillInfo();

	void				Appear();
protected:
	virtual HRESULT Ready_Components();

private:
	void		Add_Hit();
	void		Follow_Target(_float fTimeDelta);

	void		Effect_Explosion();
	void		Effect_Shooting(_float fTimeDelta);

private:
	_bool	m_bExplosion = false;
	_float	m_fCurrTime = 0.0f;
	_float	m_fExplosionTime = 0.0f;
	_float	m_fSkillTime = 0.0f;
	_float	m_fSendInfoTime = 0.0f;


	_float	m_fEffectTime = 0.0f;
	_float	m_fCurrEffectTime = 0.0f;

	_uint	m_iAttack = 0;

	CFireBall* m_pFireBall = nullptr;
	CFireBallPlane* m_pPlane = nullptr;
	CFireBallTrail* m_pTrail = nullptr;

public:
	static	CSkill_FireBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

