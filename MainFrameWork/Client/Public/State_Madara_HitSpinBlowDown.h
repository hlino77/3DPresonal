#pragma once
#include "State.h"

BEGIN(Client)

class CState_Madara_HitSpinBlowDown : public CState
{
public:
	CState_Madara_HitSpinBlowDown(const wstring& strStateName, class CBoss_Madara* pBoss);
	CState_Madara_HitSpinBlowDown(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Madara_HitSpinBlowDown() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void		 Knock_Back();
	void		 LookAt_HitObject(_float fTimeDelta);

private:
	class CBoss_Madara* m_pBoss = nullptr;

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