#pragma once
#include "Player.h"

BEGIN(Client)

class CLineCircle;


class CSkill_Rasengun;
class CSkill_RasenSyuriken;


class CPlayer_Naruto : public CPlayer
{
public:
	enum NARUTO_SKILL { RASENGAN, RASENSYURIKEN, SKILLEND };

private:
	CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Naruto(const CPlayer_Naruto& rhs);
	virtual ~CPlayer_Naruto() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();



	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther) override;
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther) override;

	virtual	void	OnCollisionEnter_NoneControl(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionExit_NoneControl(const _uint iColLayer, class CCollider* pOther);
	virtual void	Set_Skill(CGameObject* pGameObject) override;


public:
	void				Send_PlayerInfo();

	void				Set_Colliders(_float fTimeDelta);

	void				Effect_Hit();


	CSkill_Rasengun*	Get_Rasengun() { return m_pRasengun; }
	CSkill_RasenSyuriken* Get_RasenSyuriken() { return m_pRasenSyuriken; }

protected:
	virtual HRESULT Ready_Components() override;
	HRESULT			Ready_State();
	HRESULT			Ready_Coliders();


private:
	HRESULT			Ready_SkillUI();


private:
	_float	m_fSendInfoTime = 0.0f;



	CSkill_Rasengun* m_pRasengun = nullptr;
	CSkill_RasenSyuriken* m_pRasenSyuriken = nullptr;


public:
	static CPlayer_Naruto* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END