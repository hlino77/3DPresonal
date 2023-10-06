#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_Attack_cmb08 : public CState
{
public:
	CState_Sasuke_Attack_cmb08(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_Attack_cmb08(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_Attack_cmb08() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);

	void		Update_Collider(_float fTimeDelta);

	void		Follow_TargetObject(_float fTimeDelta);
	void		Follow_TargetPos(_float fTimeDelta);
	void		Set_TargetPos();
private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

	std::function<void(CState_Sasuke_Attack_cmb08&, _float)> m_TickFunc;
private:
	_int	m_iAnimIndex = 0;
	_uint	m_iChangeFrame = 0;
	_uint	m_iInputNext = 0;
	_uint	m_iStartFrame = 0;
	_uint	m_iNextFrame = 0;
	_uint	m_iColliderFrame = 0;


	_float	m_fMoveSpeed = 0.0f;

public:
	virtual void Free() override;
};

END