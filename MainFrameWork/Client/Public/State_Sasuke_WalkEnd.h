#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_WalkEnd : public CState
{
public:
	CState_Sasuke_WalkEnd(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_WalkEnd(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_WalkEnd() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT		Initialize() override;
	virtual void		Enter_State() override;
	virtual void		Tick_State(_float fTimeDelta) override;
	virtual void		Exit_State() override;


	void				Tick_State_Control(_float fTimeDelta);
	void				Tick_State_NoneControl(_float fTimeDelta);

private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;
	
	std::function<void(CState_Sasuke_WalkEnd&, _float)> m_TickFunc;

private:
	//Animation
	_int m_iWalk_End = -1;

public:
	virtual void Free() override;
};

END