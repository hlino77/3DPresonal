#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)
class CFireBall;
class CFireBallPlane;
class CFireBallTrail;


class CSkill_MadaraFireBall : public CSkill
{
protected:
	CSkill_MadaraFireBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_MadaraFireBall(const CSkill_MadaraFireBall& rhs);
	virtual ~CSkill_MadaraFireBall() = default;

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
	void				Appear();

protected:
	virtual HRESULT Ready_Components();

private:
	void		Follow_Target(_float fTimeDelta);

	void		Effect_Explosion();
	void		Effect_Shooting(_float fTimeDelta);

private:
	_bool	m_bExplosion = false;

	_float	m_fEffectTime = 0.0f;
	_float	m_fCurrEffectTime = 0.0f;

	_uint	m_iAttack = 0;

	CFireBall* m_pFireBall = nullptr;
	CFireBallPlane* m_pPlane = nullptr;
	CFireBallTrail* m_pTrail = nullptr;
public:
	static	CSkill_MadaraFireBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

