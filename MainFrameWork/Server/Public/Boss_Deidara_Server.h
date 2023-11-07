#pragma once

#include "Server_Defines.h"
#include "Boss_Server.h"


BEGIN(Server)
class CMonster_C2Dragon_Server;
class CSkill_TwinBird_Server;


class CBoss_Deidara_Server : public CBoss_Server
{
public:
	typedef struct SkillInfoTag
	{
		_float m_fCoolTime;
		_float m_fCurrCoolTime;
		_bool m_bReady;
	}SKILLINFO;

	enum DEIDARA_SKILL { C2DRAGON, TWINBIRD, SKILLEND };


protected:
	CBoss_Deidara_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Deidara_Server(const CBoss_Deidara_Server& rhs);
	virtual ~CBoss_Deidara_Server() = default;

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

	void				Attack_Random();

	void				Set_C2Dragon(CMonster_C2Dragon_Server* pDragon) { m_pC2Dragon = pDragon; }
	void				Spawn_C2Dragon();
	void				Shoot_TwinBirds();


	_bool				Get_SkillReady(DEIDARA_SKILL eSkill) { return m_SkillInfo[eSkill].m_bReady; }


	void				Reset_FindTargetTime() { m_fFindTargetTime = 0.0f; }


	const wstring&		Get_CurrState() { return m_pStateMachine->Get_CurrState(); }

	virtual void		Set_Die();
protected:
	virtual HRESULT		Ready_Components();

private:
	void				Ready_State();
	void				Update_Skill(SKILLINFO& tSkill, _float fTimeDelta);

private:
	_float	m_fSendInfoTime = 0.0f;

	_float	m_fFindTargetTime = 0.0f;

	
	
	
	//C2Dragon
	CMonster_C2Dragon_Server* m_pC2Dragon = nullptr;

	CSkill_TwinBird_Server* m_pTwinBird[2];

	vector<SKILLINFO> m_SkillInfo;


public:
	static	CBoss_Deidara_Server* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free();


};

END

