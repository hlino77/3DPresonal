#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)

class CRasenSyurikenBlade;
class CRasenSyurikenBladeBlur;
class CRasenSyurikenBladeLine;
class CRasenSyurikenCenter;

class CSkill_RasenSyuriken : public CSkill
{
protected:
	CSkill_RasenSyuriken(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_RasenSyuriken(const CSkill_RasenSyuriken& rhs);
	virtual ~CSkill_RasenSyuriken() = default;

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
	void				Shoot();
protected:
	virtual HRESULT Ready_Components();

private:
	void		Effect_SmokeRing(_float fTime, _float fBaseScale);


	void		Add_Hit();
	void		Follow_Target(_float fTimeDelta);

	void		Effect_Explosion();

	void		Update_Transform();
private:
	_bool	m_bExplosion = false;
	_float	m_fCurrTime = 0.0f;
	_float	m_fExplosionTime = 0.0f;
	_float	m_fSkillTime = 0.0f;
	_float	m_fSendInfoTime = 0.0f;


	_float	m_fEffectTime = 0.0f;
	_float	m_fCurrEffectTime = 0.0f;

	_uint	m_iAttack = 0;


	_bool m_bShoot = false;

	_uint m_iBoneIndex = 0;

	CRasenSyurikenBlade* m_pBlade;
	CRasenSyurikenBladeBlur* m_pBlur;
	CRasenSyurikenBladeLine* m_pLine;
	CRasenSyurikenCenter* m_pCenter;
public:
	static	CSkill_RasenSyuriken* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

