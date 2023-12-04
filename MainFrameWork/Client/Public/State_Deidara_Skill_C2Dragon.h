#pragma once
#include "State.h"

BEGIN(Client)

class CState_Deidara_Skill_C2Dragon : public CState
{
public:
	CState_Deidara_Skill_C2Dragon(const wstring& strStateName, class CBoss_Deidara* pBoss);
	CState_Deidara_Skill_C2Dragon(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_Skill_C2Dragon() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Deidara* m_pBoss = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;


	_bool m_bDragon = false;
	_bool m_bSound = false;
public:
	virtual void Free() override;
};

END