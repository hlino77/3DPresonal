#pragma once
#include "State.h"

BEGIN(Server)

class CState_WhiteZetsu_GetUp_Server : public CState
{
public:
	CState_WhiteZetsu_GetUp_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster);
	CState_WhiteZetsu_GetUp_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_GetUp_Server() = default;


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
	_int m_iDownLoop = 0;
	_int m_iGetUp = 0;

	_bool m_bLoopEnd = false;
public:
	virtual void Free() override;
};

END