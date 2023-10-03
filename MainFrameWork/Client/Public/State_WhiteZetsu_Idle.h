#pragma once
#include "State.h"

BEGIN(Client)

class CState_WhiteZetsu_Idle : public CState
{
public:
	CState_WhiteZetsu_Idle(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster);
	CState_WhiteZetsu_Idle(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_Idle() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CMonster_WhiteZetsu* m_pMonster = nullptr;

private:
	//Animation
	_int m_iIdle_Loop = 0;

public:
	virtual void Free() override;
};

END