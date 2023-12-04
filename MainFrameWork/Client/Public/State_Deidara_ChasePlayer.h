#pragma once
#include "State.h"

BEGIN(Client)

class CState_Deidara_ChasePlayer : public CState
{
public:
	CState_Deidara_ChasePlayer(const wstring& strStateName, class CBoss_Deidara* pBoss);
	CState_Deidara_ChasePlayer(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_ChasePlayer() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	void				Update_MoveSound();
	
private:
	class CBoss_Deidara* m_pBoss = nullptr;

private:
	//Animation
	_int m_iRun_Loop = 0;

	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;
	_bool m_bMoveSound = false;
public:
	virtual void Free() override;
};

END