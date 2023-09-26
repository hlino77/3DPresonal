#pragma once
#include "State.h"

class CState_Sasuke_Idle : public CState
{
public:
	CState_Sasuke_Idle(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_Idle(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_Idle() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

private:
	//Animation
	_int m_iIdle_Loop = 0;

public:
	virtual void Free() override;
};

