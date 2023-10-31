#pragma once
#include "State.h"

BEGIN(Server)

class CState_Deidara_Idle_Server : public CState
{
public:
	CState_Deidara_Idle_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss);
	CState_Deidara_Idle_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_Idle_Server() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Deidara_Server* m_pBoss = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;

public:
	virtual void Free() override;
};

END