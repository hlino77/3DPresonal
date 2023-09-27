#pragma once
#include "State.h"

BEGIN(Client)

class CState_NoneControl_Idle : public CState
{
public:
	CState_NoneControl_Idle(const wstring& strStateName, class CPlayer* pPlayer);
	CState_NoneControl_Idle(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_NoneControl_Idle() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer* m_pPlayer = nullptr;

private:


public:
	virtual void Free() override;
};

END