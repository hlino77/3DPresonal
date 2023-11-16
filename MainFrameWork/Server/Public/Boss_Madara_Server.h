#pragma once

#include "Server_Defines.h"
#include "Boss_Server.h"


BEGIN(Server)
class CMonster_C2Dragon_Server;
class CSkill_TwinBird_Server;


class CBoss_Madara_Server : public CBoss_Server
{
public:
	typedef struct SkillInfoTag
	{
		_float m_fCoolTime;
		_float m_fCurrCoolTime;
		_bool m_bReady;
	}SKILLINFO;

	enum MADARA_SKILL { C2DRAGON, TWINBIRD, SKILLEND };


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


	void				Attack_Random();



	void				Reset_FindTargetTime() { m_fFindTargetTime = 0.0f; }


	const wstring&		Get_CurrState() { return m_pStateMachine->Get_CurrState(); }

	virtual void		Set_Die();


	void				Set_NormalAttackHit(_bool bHit) { m_bNormalAttackHit = bHit; }
	_bool				Is_NormalAttackHit() { return m_bNormalAttackHit; }
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


public:
	static	CBoss_Madara_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END
