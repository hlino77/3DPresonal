#pragma once
#include "Player.h"

BEGIN(Client)
class CSkill_Chidori;
class CSkill_FireBall;

class CPlayer_Sasuke : public CPlayer
{
public:
	enum SASUKE_SKILL { CHIDORI, FIREBALL, SKILLEND };


private:
	CPlayer_Sasuke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer_Sasuke(const CPlayer_Sasuke& rhs);
	virtual ~CPlayer_Sasuke() = default;

public:
	virtual HRESULT		Initialize_Prototype();
	virtual HRESULT		Initialize(void* pArg);
	virtual void		Tick(_float fTimeDelta);
	virtual void		LateTick(_float fTimeDelta);
	virtual HRESULT		Render();


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


	CSkill_Chidori*		Get_Chidori() { return m_pChidori; }
	CSkill_FireBall*	Get_FireBall() { return m_pFireBall; }

private:
	HRESULT			Ready_SkillUI();

protected:
	virtual HRESULT Ready_Components() override;
	HRESULT			Ready_State();
	HRESULT			Ready_Coliders();



private:
	_float	m_fSendInfoTime = 0.0f;
	_bool m_bHitEffect = false;


	CSkill_Chidori* m_pChidori = nullptr;
	CSkill_FireBall* m_pFireBall = nullptr;


public:
	static CPlayer_Sasuke* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;


};

END