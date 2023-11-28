#pragma once

#include "Server_Defines.h"
#include "Skill_Server.h"


BEGIN(Server)
class CSkill_MadaraFireBall_Server : public CSkill_Server
{
protected:
	CSkill_MadaraFireBall_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkill_MadaraFireBall_Server(const CSkill_MadaraFireBall_Server& rhs);
	virtual ~CSkill_MadaraFireBall_Server() = default;

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

	void				Appear();

	void		Follow_Target(_float fTimeDelta);

protected:
	virtual HRESULT Ready_Components();


private:
	_float	m_fCurrTime = 0.0f;
	_float	m_fExplosionTime = 0.2f;
	_float	m_fSkillTime = 0.0f;

	_bool	m_bExplosion = false;

	std::future<HRESULT>			m_PlayAnimation;


	_float m_fCurrScale = 0.0f;
public:
	static	CSkill_MadaraFireBall_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();
};

END

