#pragma once
#include "State.h"

BEGIN(Server)

class CState_WhiteZetsu_HitSpinBlowDown_Server : public CState
{
public:
	CState_WhiteZetsu_HitSpinBlowDown_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pPlayer);
	CState_WhiteZetsu_HitSpinBlowDown_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_HitSpinBlowDown_Server() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void		 Knock_Back();
	void		 LookAt_HitObject(_float fTimeDelta);

private:
	class CMonster_WhiteZetsu_Server* m_pMonster = nullptr;

	_float m_fOriginDrag = 0.0f;
	
private:
	//Animation
	_int m_iAnimIndex = 0;

	Vec3 m_vHitPos;

	_bool m_bKnockBack = false;
public:
	virtual void Free() override;
};

END