#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_WireDash_Start : public CState
{
public:
	CState_Naruto_WireDash_Start(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_WireDash_Start(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_WireDash_Start() = default;


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

	std::function<void(CState_Naruto_WireDash_Start&, _float)> m_TickFunc;

	

private:
	//Animation
	_int m_iAnimIndex = 0;

public:
	virtual void Free() override;
};

END