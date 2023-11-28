#pragma once
#include "Boss.h"

BEGIN(Client)
class CMonster_C2Dragon;
class CSkill_TwinBird;

class CBoss_Deidara : public CBoss
{
private:
	CBoss_Deidara(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Deidara(const CBoss_Deidara& rhs);
	virtual ~CBoss_Deidara() = default;

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
	void				Set_C2Dragon(CMonster_C2Dragon* pDragon) { m_pC2Dragon = pDragon; }
	void				Spawn_C2Dragon();
	void				Shoot_TwinBirds();

	virtual void		Set_Die();

	void				Effect_Hit();
protected:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_State();
	

private:
	_float	m_fSendInfoTime = 0.0f;


	CMonster_C2Dragon* m_pC2Dragon = nullptr;


	CSkill_TwinBird* m_pTwinBird[2];

	_bool m_bHitEffect = false;

public:
	static CBoss_Deidara* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END