#pragma once
#include "State.h"

BEGIN(Client)

class CState_Naruto_GetUp : public CState
{
public:
	CState_Naruto_GetUp(const wstring& strStateName, class CPlayer_Naruto* pPlayer);
	CState_Naruto_GetUp(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Naruto_GetUp() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;


	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);


private:
	class CPlayer_Naruto* m_pPlayer = nullptr;

	std::function<void(CState_Naruto_GetUp&, _float)> m_TickFunc;

	

private:
	//Animation
	_int m_iDownLoop = 0;
	_int m_iGetUp = 0;

	_bool m_bLoopEnd = false;
public:
	virtual void Free() override;
};

END