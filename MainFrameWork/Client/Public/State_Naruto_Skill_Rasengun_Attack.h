#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_Skill_Rasengun_Attack : public CState
{
public:
	CState_Naruto_Skill_Rasengun_Attack(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_Skill_Rasengun_Attack(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_Skill_Rasengun_Attack() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);


	void	LookAt_Target(_float fTimeDelta);

private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	std::function<void(CState_Naruto_Skill_Rasengun_Attack&, _float)> m_TickFunc;

	

private:
	//Animation
	_int m_iAnimIndex = 0;

	_bool m_bAttack = false;

public:
	virtual void Free() override;
};

END