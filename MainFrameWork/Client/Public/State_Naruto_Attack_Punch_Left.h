#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_Attack_Punch_Left : public CState
{
public:
	CState_Naruto_Attack_Punch_Left(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_Attack_Punch_Left(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_Attack_Punch_Left() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	void		Tick_State_Control(_float fTimeDelta);
	void		Tick_State_NoneControl(_float fTimeDelta);


	void		Update_Collider(_float fTimeDelta);

	void		Set_TargetPos();
	void		Follow_TargetPos(_float fTimeDelta);
private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	std::function<void(CState_Naruto_Attack_Punch_Left&, _float)> m_TickFunc;

private:

	_int	m_iAnimIndex = 0;
	_uint	m_iChangeFrame = 0;
	_uint	m_iInputNext = 0;
	_uint	m_iStartFrame = 0;
	_uint	m_iNextFrame = 0;
	_uint	m_iColliderFrame = 0;

	_bool	m_bAttack = false;

	_float	m_fCollierTime = 0.0f;
	_float	m_fCurrTime = 0.0f;

public:
	virtual void Free() override;
};

END