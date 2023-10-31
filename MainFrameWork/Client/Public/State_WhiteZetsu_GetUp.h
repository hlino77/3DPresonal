#pragma once
#include "State.h"

BEGIN(Client)

class CState_WhiteZetsu_GetUp : public CState
{
public:
	CState_WhiteZetsu_GetUp(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster);
	CState_WhiteZetsu_GetUp(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_GetUp() = default;


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
	_int m_iDownLoop = 0;
	_int m_iGetUp = 0;

	_bool m_bLoopEnd = false;
public:
	virtual void Free() override;
};

END