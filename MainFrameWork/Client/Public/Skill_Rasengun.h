#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)

class CRasenganCircle;
class CSmokeRing;

class CSkill_Rasengun : public CSkill
{
protected:
	CSkill_Rasengun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Rasengun(const CSkill_Rasengun& rhs);
	virtual ~CSkill_Rasengun() = default;

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

	void				Set_Run();

protected:
	virtual HRESULT Ready_Components();

private:
	void		Effect_LineCircle();
	void		Effect_SmokeRing(_float fTime, _float fBaseScale);
	void		Add_Hit();

private:
	_bool	m_bExplosion = false;
	_float	m_fCurrTime = 0.0f;
	_float	m_fExplosionTime = 0.0f;
	_float	m_fAttackTime = 0.0f;

	_bool	m_bAttackTime = false;
	_bool	m_bEffect = false;

	_float	m_fEffectDelay = 0.0f;
	_float	m_fEffectCurrTime = 0.0f;

	_float	m_fAddHitTime = 0.0f;
	_float	m_fCurrHitTime = 0.0f;

	_float	m_fSmokeRingScale = 0.0f;

	CRasenganCircle* m_pRasengunCircle = nullptr;

	_uint	m_iHitCount = 0;

	_uint	m_iAttack = 0;
public:
	static	CSkill_Rasengun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

