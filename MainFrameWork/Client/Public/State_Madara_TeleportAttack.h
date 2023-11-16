#pragma once
#include "State.h"

BEGIN(Client)

class CState_Madara_TeleportAttack : public CState
{
public:
	CState_Madara_TeleportAttack(const wstring& strStateName, class CBoss_Madara* pBoss);
	CState_Madara_TeleportAttack(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Madara_TeleportAttack() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Madara* m_pBoss = nullptr;

private:
	//Animation
	_int m_iIdle_Loop = 0;

	_bool m_bMove = false;

public:
	virtual void Free() override;
};

END