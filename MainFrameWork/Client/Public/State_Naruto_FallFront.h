#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_FallFront : public CState
{
public:
	CState_Naruto_FallFront(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_FallFront(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_FallFront() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);

private:
	void	Set_Ray();
	void	Set_Wall();


private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	std::function<void(CState_Naruto_FallFront&, _float)> m_TickFunc;

	

private:
	//Animation
	_int m_iFall_Front = 0;

public:
	virtual void Free() override;
};

END