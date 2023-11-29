#pragma once

#include "Server_Defines.h"
#include "Boss_Server.h"


BEGIN(Server)
class CSkill_Meteor_Server;
class CSkill_MadaraFireBall_Server;



class CBoss_Madara_Server : public CBoss_Server
{
public:
	enum MADARA_SKILL { METEOR, FIREBALL, SKILLEND };


protected:
	CBoss_Madara_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Madara_Server(const CBoss_Madara_Server& rhs);
	virtual ~CBoss_Madara_Server() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual	void	OnCollisionEnter(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionStay(const _uint iColLayer, class CCollider* pOther);
	virtual	void	OnCollisionExit(const _uint iColLayer, class CCollider* pOther);
	virtual void	Set_Skill(CGameObject* pObject) override;

public:
	void				Send_BossInfo();


	
	void				Update_NearTarget(_float fTimeDelta);
	_bool				Ready_TeleportAttack(_float fTimeDelta);
	void				Set_TeleportAttackDelay(_float fDelay) { m_fTeleportAttackDelay = fDelay; }


	void				Attack_Random();



	void				Reset_FindTargetTime() { m_fFindTargetTime = 0.0f; }


	const wstring&		Get_CurrState() { return m_pStateMachine->Get_CurrState(); }

	virtual void		Set_Die();

	_bool				Get_SkillReady(MADARA_SKILL eSkill) { return m_SkillInfo[eSkill].m_bReady; }
	void				Set_SkillReady(MADARA_SKILL eSkill, _bool bReady) { m_SkillInfo[eSkill].m_bReady = bReady; }
	void				ResetSkill(MADARA_SKILL eSkill);

	void				Set_NormalAttackHit(_bool bHit) { m_bNormalAttackHit = bHit; }
	_bool				Is_NormalAttackHit() { return m_bNormalAttackHit; }


	CSkill_Meteor_Server* Get_Meteor() { return m_pMeteor; }
	CSkill_MadaraFireBall_Server* Get_FireBall() { return m_pFireBall; }

	void				Set_FireBall(_bool bFireBall) { m_bFireBall = bFireBall; }
	_bool				Is_FireBall() { return m_bFireBall; }

protected:
	virtual HRESULT		Ready_Components();

private:
	void				Ready_State();
	void				Update_Skill(SKILLINFO& tSkill, _float fTimeDelta);

private:
	_float	m_fSendInfoTime = 0.0f;

	_float	m_fFindTargetTime = 0.0f;


	_float	m_fTeleportAttackDelay = 0.0f;

	_bool m_bNormalAttackHit = false;


	vector<SKILLINFO> m_SkillInfo;

	CSkill_Meteor_Server* m_pMeteor = nullptr;
	CSkill_MadaraFireBall_Server* m_pFireBall = nullptr;

	_bool m_bFireBall = false;
public:
	static	CBoss_Madara_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

