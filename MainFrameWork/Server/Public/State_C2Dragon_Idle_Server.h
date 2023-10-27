#pragma once
#include "State.h"

BEGIN(Server)

class CState_C2Dragon_Idle_Server : public CState
{
public:
	CState_C2Dragon_Idle_Server(const wstring& strStateName, class CMonster_C2Dragon_Server* pMonster);
	CState_C2Dragon_Idle_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_C2Dragon_Idle_Server() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:

	void		Update_Attack(_float fTimeDelta);
private:
	class CMonster_C2Dragon_Server* m_pMonster = nullptr;

private:
	//Animation
	_int m_iIdle_Loop = 0;


	_float m_fCurrAttackTime;


	_bool	m_bUpdateTarget = false;
	_float m_fAttackDelay;
public:
	virtual void Free() override;
};

END