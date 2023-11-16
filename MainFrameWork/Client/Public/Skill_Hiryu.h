#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)
class CSkill_Hiryu : public CSkill
{
protected:
	CSkill_Hiryu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Hiryu(const CSkill_Hiryu& rhs);
	virtual ~CSkill_Hiryu() = default;

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

	void				Shoot_Hiryu(CGameObject* pTargetObject, Vec3 vPos, Vec3 vLook);

	
protected:
	virtual HRESULT Ready_Components();


private:
	CShader* m_pShaderCom = nullptr;

	_float	m_fCurrTime = 0.0f;
	_float	m_fTimer = 0.0f;
	_float	m_fExplosionTime = 0.2f;

	_bool	m_bExplosion = false;


	std::future<HRESULT>			m_PlayAnimation;


	_float m_fCurrScale = 0.0f;
public:
	static	CSkill_Hiryu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

