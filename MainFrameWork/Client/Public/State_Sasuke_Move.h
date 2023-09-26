#pragma once
#include "State.h"

class CState_Sasuke_Move : public CState
{
public:
	enum RUNSTATE { RUNSTART, RUNEND };
public:
	CState_Sasuke_Move(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_Move(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_Move() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void		UpdateMove(_float fTimeDelta);
	void		UpdateEnd(_float fTimeDelta);
private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

private:
	RUNSTATE m_eState;
	_bool m_bStop = false;
	_float m_fCurrSpeed = 0.f;
	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;



	//Animation
	_int m_iRun_Loop = -1;
	_int m_iRun_End = -1;
	

public:
	virtual void Free() override;
};

