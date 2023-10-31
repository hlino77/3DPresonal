#pragma once
#include "State.h"

BEGIN(Client)

class CState_Deidara_Dying_Normal : public CState
{
public:
	CState_Deidara_Dying_Normal(const wstring& strStateName, class CBoss_Deidara* pBoss);
	CState_Deidara_Dying_Normal(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_Dying_Normal() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Deidara* m_pBoss = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;


	_float m_fAttackDelay;
public:
	virtual void Free() override;
};

END