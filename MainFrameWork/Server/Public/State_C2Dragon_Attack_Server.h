#pragma once
#include "State.h"

BEGIN(Server)

class CState_C2Dragon_Attack_Server : public CState
{
public:
	CState_C2Dragon_Attack_Server(const wstring& strStateName, class CMonster_C2Dragon_Server* pMonster);
	CState_C2Dragon_Attack_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_C2Dragon_Attack_Server() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:


private:
	class CMonster_C2Dragon_Server* m_pMonster = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;

	_bool m_bShoot = false;
public:
	virtual void Free() override;
};

END