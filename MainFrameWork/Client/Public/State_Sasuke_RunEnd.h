#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_RunEnd : public CState
{
public:
	CState_Sasuke_RunEnd(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_RunEnd(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_RunEnd() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);

private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

	std::function<void(CState_Sasuke_RunEnd&, _float)> m_TickFunc;

private:
	//Animation
	_int m_iRun_End = -1;
	DirectX::SimpleMath::Vector3 m_vStopPos;

public:
	virtual void Free() override;
};

END