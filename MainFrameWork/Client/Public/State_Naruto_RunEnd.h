#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_RunEnd : public CState
{
public:
	CState_Naruto_RunEnd(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_RunEnd(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_RunEnd() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

private:
	//Animation
	_int m_iRun_End = -1;
	DirectX::SimpleMath::Vector3 m_vStopPos;

public:
	virtual void Free() override;
};

END