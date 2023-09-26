#pragma once
#include "State.h"

class CState_Naruto_Attack_Normal : public CState
{
public:
	typedef struct AnimationActionKey
	{
		_int iAnimIndex = 0;
		_uint iChangeFrame = 0;
		_uint iInputNext = 0;
		_uint iNextFrame = 0;
	}ACTIONKEY;

public:
	CState_Naruto_Attack_Normal(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_Attack_Normal(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_Attack_Normal() = default;


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
	ACTIONKEY m_iAttack_Punch_Left;
	ACTIONKEY m_iAttack_Punch_Right;
	ACTIONKEY m_iAttack_ElbowStrike;
	ACTIONKEY m_iAttack_JumpDoubleKick;

	vector<ACTIONKEY> m_Attack;
	_int m_iCurrAttack = 0;

public:
	virtual void Free() override;
};

