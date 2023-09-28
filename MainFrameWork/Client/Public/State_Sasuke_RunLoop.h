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

	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);


private:
	void	Set_TargetPos();
private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;


	std::function<void(CState_Sasuke_RunLoop&, _float)> m_TickFunc;
private:
	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;



	//Animation
	_int m_iRun_Loop = -1;
	

public:
	virtual void Free() override;
};

END