#pragma once
#include "State.h"

BEGIN(Client)

class CState_Madara_Die : public CState
{
public:
	CState_Madara_Die(const wstring& strStateName, class CBoss_Madara* pBoss);
	CState_Madara_Die(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Madara_Die() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Madara* m_pBoss = nullptr;

private:

public:
	virtual void Free() override;
};

END