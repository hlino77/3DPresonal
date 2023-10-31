#pragma once
#include "State.h"

BEGIN(Server)

class CState_Deidara_ChasePlayer_Server : public CState
{
public:
	CState_Deidara_ChasePlayer_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss);
	CState_Deidara_ChasePlayer_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_ChasePlayer_Server() = default;


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
	_int m_iRun_Loop = 0;

	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;

public:
	virtual void Free() override;
};

END