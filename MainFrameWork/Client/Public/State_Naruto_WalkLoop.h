#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_WalkLoop : public CState
{
public:
	CState_Naruto_WalkLoop(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_WalkLoop(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_WalkLoop() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT		Initialize() override;
	virtual void		Enter_State() override;
	virtual void		Tick_State(_float fTimeDelta) override;
	virtual void		Exit_State() override;


	void				Tick_State_Control(_float fTimeDelta);
	void				Tick_State_NoneControl(_float fTimeDelta);
private:
	void				Set_TargetPos(Vec3 vDir);
private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	
	std::function<void(CState_Naruto_WalkLoop&, _float)> m_TickFunc;
private:
	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;



	//Animation
	_int m_iWalk_Loop = -1;
	

	Vec3 Make_MoveDir();


public:
	virtual void Free() override;
};

END