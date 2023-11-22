#pragma once

#include "Client_Defines.h"
#include "Skill.h"


BEGIN(Client)

class CMeteor_HemiSphere;
class CMeteor_Bottom;
class CMeteor_Smoke;
class CMeteor_Cylinder;

class CSkill_Meteor : public CSkill
{
protected:
	CSkill_Meteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_Meteor(const CSkill_Meteor& rhs);
	virtual ~CSkill_Meteor() = default;

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
	CShader* m_pShaderCom = nullptr;

private:
	_bool	m_bExplosion = false;

	CMeteor_HemiSphere* m_pHemiSphere = nullptr;
	CMeteor_Bottom* m_pBottom = nullptr;
	CMeteor_Smoke* m_pSmoke = nullptr;
	CMeteor_Cylinder* m_pCylinder = nullptr;


	Vec3 m_vEffectPos;
	Vec3 m_vStartPos;
public:
	static	CSkill_Meteor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

