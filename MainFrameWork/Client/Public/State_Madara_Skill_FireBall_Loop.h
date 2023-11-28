#pragma once
#include "State.h"

BEGIN(Client)

class CState_Madara_Skill_FireBall_Loop : public CState
{
public:
	CState_Madara_Skill_FireBall_Loop(const wstring& strStateName, class CBoss_Madara* pBoss);
	CState_Madara_Skill_FireBall_Loop(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Madara_Skill_FireBall_Loop() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

private:
	class CBoss_Madara* m_pBoss = nullptr;

private:
	//Animation
	_int m_iAnimIndex = 0;

public:
	virtual void Free() override;
};

END