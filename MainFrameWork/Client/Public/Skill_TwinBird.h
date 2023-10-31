#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)
class CSkill_TwinBird : public CSkill
{
protected:
	CSkill_TwinBird(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_TwinBird(const CSkill_TwinBird& rhs);
	virtual ~CSkill_TwinBird() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther);


	virtual HRESULT					Ready_Coliders();

public:
	void				Send_SkillInfo();

	void				Shoot_TwinBird(CGameObject* pTargetObject, Vec3 vPos, Vec3 vLook);

	void				Explosion();

protected:
	virtual HRESULT Ready_Components();


private:
	CShader* m_pShaderCom = nullptr;

	_float	m_fCurrTime = 0.0f;
	_float	m_fTimer = 0.0f;
	_float	m_fExplosionTime = 0.2f;
	_float	m_fLookSpeed = 0.0f;
	_bool	m_bExplosion = false;


	_int	m_iFly_Loop = 0;
	_int	m_iFly_Start = 0;
	_bool	m_bFlyLoop = false;

	std::future<HRESULT>			m_PlayAnimation;
public:
	static	CSkill_TwinBird* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

