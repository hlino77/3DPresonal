#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_Attack_cmb03 : public CState
{
public:
	CState_Sasuke_Attack_cmb03(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_Attack_cmb03(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_Attack_cmb03() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

private:
	_int	m_iAnimIndex = 0;
	_uint	m_iChangeFrame = 0;
	_uint	m_iInputNext = 0;
	_uint	m_iStartFrame = 0;
	_uint	m_iNextFrame = 0;

public:
	virtual void Free() override;
};

END