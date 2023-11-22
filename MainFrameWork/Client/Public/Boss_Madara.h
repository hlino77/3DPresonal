#pragma once
#include "Boss.h"

BEGIN(Client)

class CSkill_Meteor;

class CBoss_Madara : public CBoss
{
private:
	CBoss_Madara(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Madara(const CBoss_Madara& rhs);
	virtual ~CBoss_Madara() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		LateTick(_float fTimeDelta);
	virtual HRESULT		Render();
	virtual void		Set_Skill(CGameObject* pGameObject) override;

	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;


	void				Set_Colliders();
	

	virtual void		Set_Die();


	CSkill_Meteor* Get_Meteor() { return m_pMeteor; }

protected:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_State();
	
private:
	void				Effect_Hit();


private:
	_float	m_fSendInfoTime = 0.0f;



	_bool m_bHitEffect = false;


	CSkill_Meteor* m_pMeteor = nullptr;
public:
	static CBoss_Madara* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END