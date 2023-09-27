#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_RunLoop : public CState
{
public:
	CState_Sasuke_RunLoop(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_RunLoop(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_RunLoop() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

private:
	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;



	//Animation
	_int m_iRun_Loop = -1;
	

public:
	virtual void Free() override;
};

END