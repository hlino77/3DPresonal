#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_RunLoop : public CState
{
public:
	struct SplinePos
	{
		Vec3 vPos, vTargetPos;
	};

public:
	CState_Naruto_RunLoop(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_RunLoop(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_RunLoop() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT		Initialize() override;
	virtual void		Enter_State() override;
	virtual void		Tick_State(_float fTimeDelta) override;
	virtual void		Exit_State() override;


	void				Tick_State_Control(_float fTimeDelta);
	void				Tick_State_NoneControl(_float fTimeDelta);
private:
	Vec3				Make_MoveDir();
	void				Set_TargetPos(Vec3 vDir);
private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	
	std::function<void(CState_Naruto_RunLoop&, _float)> m_TickFunc;
private:
	_float m_fMaxSpeed = 0.f;
	_float m_fAccel = 0.f;



	//Animation
	_int m_iRun_Loop = -1;
	



	//NoneControl
	Vec3 m_vPrevPos;


	_float m_fSplineRatio;
	deque<SplinePos> m_Spline;

	void	Push_Spline();
public:
	virtual void Free() override;
};

END