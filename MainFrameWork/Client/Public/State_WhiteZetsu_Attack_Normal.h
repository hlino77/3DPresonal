#pragma once
#include "State.h"

BEGIN(Client)

class CState_WhiteZetsu_Attack_Normal : public CState
{
public:
	CState_WhiteZetsu_Attack_Normal(const wstring& strStateName, class CMonster_WhiteZetsu* pPlayer);
	CState_WhiteZetsu_Attack_Normal(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_Attack_Normal() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CMonster_WhiteZetsu* m_pMonster = nullptr;

private:
	//Animation
	_int m_iAttack_DownStrike = 0;

public:
	virtual void Free() override;
};

END