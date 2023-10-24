#pragma once
#include "State.h"

BEGIN(Server)

class CState_WhiteZetsu_Dying_Normal_Server : public CState
{
public:
	CState_WhiteZetsu_Dying_Normal_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pPlayer);
	CState_WhiteZetsu_Dying_Normal_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_Dying_Normal_Server() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CMonster_WhiteZetsu_Server* m_pMonster = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;


	_float m_fAttackDelay;
public:
	virtual void Free() override;
};

END