#pragma once
#include "State.h"

BEGIN(Client)

class CState_Deidara_Attack_cmb02 : public CState
{
public:
	CState_Deidara_Attack_cmb02(const wstring& strStateName, class CBoss_Deidara* pBoss);
	CState_Deidara_Attack_cmb02(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Deidara_Attack_cmb02() = default;


public:
	// CState을(를) 통해 상속됨
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void			Follow_TargetPos(_float fTimeDelta);

private:
	class CBoss_Deidara* m_pBoss = nullptr;


private:
	//Animation
	_int m_iAnimIndex = 0;

public:
	virtual void Free() override;
};

END