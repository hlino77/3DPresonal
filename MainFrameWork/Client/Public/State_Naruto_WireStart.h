#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_WireStart : public CState
{
public:
	CState_Naruto_WireStart(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_WireStart(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_WireStart() = default;


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

	std::function<void(CState_Naruto_WireStart&, _float)> m_TickFunc;

	
	_bool	WirePicking();



private:
	//Animation
	_int m_iAnimIndex = 0;
	_int m_iBoneIndex = 0;


	std::future<_bool>			m_WirePicking;

	_bool m_bPickStart = false;
	_bool m_bPicked = false;

public:
	virtual void Free() override;
};

END