#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_Attack_JumpDoubleKick : public CState
{
public:
	CState_Naruto_Attack_JumpDoubleKick(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_Attack_JumpDoubleKick(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_Attack_JumpDoubleKick() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

private:
	//Animation
	_int	m_iAnimIndex = 0;
	_uint	m_iChangeFrame = 0;
	_uint	m_iInputNext = 0;
	_uint	m_iStartFrame = 0;
	_uint	m_iNextFrame = 0;

public:
	virtual void Free() override;
};

END