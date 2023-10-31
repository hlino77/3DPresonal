#pragma once
#include "State.h"
#include "Server_Defines.h"

BEGIN(Server)

class CState_Deidara_Attack_cmb01_Server : public CState
{
public:
	CState_Deidara_Attack_cmb01_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss);
	CState_Deidara_Attack_cmb01_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_Attack_cmb01_Server() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void			Update_Collider(_float fTimeDelta, _uint iColliderFrame, COLLIDER_ATTACK eAttackType);

	void			Follow_TargetPos(_float fTimeDelta);
	void			Set_TargetPos();

private:
	class CBoss_Deidara_Server* m_pBoss = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;

	_uint m_iColliderFrame1 = 0;
	_uint m_iColliderFrame2 = 0;


	_uint	m_iNextFrame = 0;


	_bool	m_bSetTargetPos = false;
	_bool	m_bAttack = false;

	_float	m_fCollierTime = 0.0f;
	_float	m_fCurrTime = 0.0f;
public:
	virtual void Free() override;
};

END