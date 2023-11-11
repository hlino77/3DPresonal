#pragma once
#include "State.h"

BEGIN(Client)

class CState_WhiteZetsu_Appear : public CState
{
public:
	CState_WhiteZetsu_Appear(const wstring& strStateName, class CMonster_WhiteZetsu* pPlayer);
	CState_WhiteZetsu_Appear(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_Appear() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;



	void		Set_LookAt_Player();
private:
	class CMonster_WhiteZetsu* m_pMonster = nullptr;

private:
	//Animation
	_int m_iAppear = 0;

	_bool m_bLookAt = false;
public:
	virtual void Free() override;
};

END