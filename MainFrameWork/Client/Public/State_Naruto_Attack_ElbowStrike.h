#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_Attack_ElbowStrike : public CState
{
public:
	CState_Naruto_Attack_ElbowStrike(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_Attack_ElbowStrike(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_Attack_ElbowStrike() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void				Tick_State_Control(_float fTimeDelta);
	void				Tick_State_NoneControl(_float fTimeDelta);
private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	std::function<void(CState_Naruto_Attack_ElbowStrike&, _float)> m_TickFunc;
private:
	_int	m_iAnimIndex = 0;
	_uint	m_iChangeFrame = 0;
	_uint	m_iInputNext = 0;
	_uint	m_iStartFrame = 0;
	_uint	m_iNextFrame = 0;

public:
	virtual void Free() override;
};

END