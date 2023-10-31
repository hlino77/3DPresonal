#pragma once
#include "State.h"

BEGIN(Server)

class CState_WhiteZetsu_Attack_Kick_Server : public CState
{
public:
	CState_WhiteZetsu_Attack_Kick_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pPlayer);
	CState_WhiteZetsu_Attack_Kick_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_WhiteZetsu_Attack_Kick_Server() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void			Update_Collider(_float fTimeDelta, _uint iColliderFrame);
private:
	class CMonster_WhiteZetsu_Server* m_pMonster = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;

	_uint m_iColliderFrame1 = 0;
	_uint m_iColliderFrame2 = 0;


	_uint	m_iNextFrame = 0;

	_bool	m_bAttack = false;

	_float	m_fCollierTime = 0.0f;
	_float	m_fCurrTime = 0.0f;
public:
	virtual void Free() override;
};

END