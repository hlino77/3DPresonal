#pragma once
#include "State.h"

BEGIN(Server)

class CState_Madara_TeleportAttack_Server : public CState
{
public:
	CState_Madara_TeleportAttack_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss);
	CState_Madara_TeleportAttack_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Madara_TeleportAttack_Server() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Madara_Server* m_pBoss = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;

	_bool m_bMove = false;

	_float m_fDelay = 0.0f;

public:
	virtual void Free() override;
};

END