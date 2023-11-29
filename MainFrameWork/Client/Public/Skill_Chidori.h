#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)

class CChidoriCenter;
class CLightningTrail;

class CSkill_Chidori : public CSkill
{
public:
	enum class CHIDORISTATE { CHARGE, RUN, EXPLOSION, HITEXPLOSION, STATEEND };

protected:
	CSkill_Chidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Chidori(const CSkill_Chidori& rhs);
	virtual ~CSkill_Chidori() = default;

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


	void				AppearTrail();
	void				DisAppearTrail();
	void				Update_TrailPoint(_float fTimeDelta);
	void				New_TargetTrailPoint(_uint iIndex);

	void				ChidoriEffect();
	void				Make_Lightning(Vec3 vPos, _uint iIndex);

	void				Set_ChidoriState(CHIDORISTATE eState);



protected:
	virtual HRESULT Ready_Components();


private:
	HRESULT Ready_Trails();
	void				Add_Hit();
	void		CamShake();
private:
	_bool	m_bExplosion = false;
	_float	m_fCurrTime = 0.0f;
	_float	m_fExplosionTime = 0.0f;
	_float	m_fAttackTime = 0.0f;

	_bool	m_bAttackTime = false;
	_bool	m_bEffect = false;

	_float	m_fEffectDelay = 0.0f;
	_float	m_fEffectCurrTime = 0.0f;

	_uint	m_iHitCount = 0;

	_uint	m_iAttack = 0;

	CChidoriCenter* m_pChidoriCenter = nullptr;


	CHIDORISTATE m_eState;

	CLightningTrail*	m_pTrail[5];
	Vec3*				m_pTrailPos[5];
	Vec3				m_vTrailCurrPos[5];
	Vec3				m_vTrailTargetPos[5];
public:
	static	CSkill_Chidori* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

