#pragma once
#include "State.h"

BEGIN(Server)

class CState_Madara_Die_Server : public CState
{
public:
	CState_Madara_Die_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss);
	CState_Madara_Die_Server(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Madara_Die_Server() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Madara_Server* m_pBoss = nullptr;

private:

public:
	virtual void Free() override;
};

END