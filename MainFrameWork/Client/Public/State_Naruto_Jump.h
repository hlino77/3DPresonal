#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_Jump : public CState
{
public:
	CState_Naruto_Jump(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_Jump(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_Jump() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);

private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	std::function<void(CState_Naruto_Jump&, _float)> m_TickFunc;

	
	void	Set_TargetPos();
	void	Set_Jump();
private:
	//Animation
	_int m_iJump_Front = 0;
	_int m_iJump_Vertical = 0;

public:
	virtual void Free() override;
};

END