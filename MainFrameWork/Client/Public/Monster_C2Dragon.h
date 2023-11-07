#pragma once
#include "Monster.h"

BEGIN(Client)

class CSkill_Hiryu;

class CMonster_C2Dragon : public CMonster
{
private:
	CMonster_C2Dragon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster_C2Dragon(const CMonster_C2Dragon& rhs);
	virtual ~CMonster_C2Dragon() = default;

public:
	virtual HRESULT		Initialize_Prototype() override;
	virtual HRESULT		Initialize(void* pArg) override;
	virtual void		Tick(_float fTimeDelta) override;
	virtual void		LateTick(_float fTimeDelta) override;
	virtual HRESULT		Render() override;
	virtual void		Set_Skill(CGameObject* pGameObject) override;


	void				Set_Colliders();


	void				Spawn();
	void				Shoot_Hiryu();

	virtual void			Set_Die();
protected:
	virtual HRESULT Ready_Components() override;
	HRESULT Ready_State();

private:
	void				Set_ToDeidara();

private:
	_float	m_fSendInfoTime = 0.0f;


	_float m_fSpawnTime = 0.0f;

	CSkill_Hiryu* m_pHiryu = nullptr;


	
public:
	static CMonster_C2Dragon* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END