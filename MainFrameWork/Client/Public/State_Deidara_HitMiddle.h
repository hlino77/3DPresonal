#pragma once
#include "State.h"

BEGIN(Client)

class CState_Deidara_HitMiddle : public CState
{
public:
	CState_Deidara_HitMiddle(const wstring& strStateName, class CBoss_Deidara* pBoss);
	CState_Deidara_HitMiddle(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_HitMiddle() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void		 Knock_Back();
	void		 LookAt_HitObject(_float fTimeDelta);
private:
	class CBoss_Deidara* m_pBoss = nullptr;

	
private:
	//Animation
	_int m_iAnimIndex = 0;

	Vec3 m_vHitPos;

	_bool m_bKnockBack = false;
public:
	virtual void Free() override;
};

END