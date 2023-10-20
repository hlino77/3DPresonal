#pragma once
#include "State.h"

BEGIN(Client)

class CState_WhiteZetsu_Attack_Punch : public CState
{
public:
	CState_WhiteZetsu_Attack_Punch(const wstring& strStateName, class CMonster_WhiteZetsu* pPlayer);
	CState_WhiteZetsu_Attack_Punch(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_Attack_Punch() = default;


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
	_int m_iAttack_Punch = 0;

public:
	virtual void Free() override;
};

END