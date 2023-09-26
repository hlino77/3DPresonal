#pragma once
#include "State.h"

class CState_Sasuke_Attack_Normal : public CState
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
	CState_Sasuke_Attack_Normal(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_Attack_Normal(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_Attack_Normal() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

private:
	//Animation
	ACTIONKEY m_iAttack_cmb01;
	ACTIONKEY m_iAttack_cmb02;
	ACTIONKEY m_iAttack_cmb03;
	ACTIONKEY m_iAttack_cmb04;

	vector<ACTIONKEY> m_Attack;
	_int m_iCurrAttack = 0;

public:
	virtual void Free() override;
};

