#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_Skill_AerialFireBall : public CState
{
public:
	CState_Sasuke_Skill_AerialFireBall(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_Skill_AerialFireBall(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_Skill_AerialFireBall() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);


private:
	void	Set_Target();
	void	LookAt_Target(_float fTimeDelta);


private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

	std::function<void(CState_Sasuke_Skill_AerialFireBall&, _float)> m_TickFunc;

private:
	//Animation
	_int m_iAnimIndex = 0;
	_uint m_iSkillFrame = 0;

	_bool m_bAttack = false;
public:
	virtual void Free() override;
};

END