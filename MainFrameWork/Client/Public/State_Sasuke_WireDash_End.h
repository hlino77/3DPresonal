#pragma once
#include "State.h"

BEGIN(Client)

class CState_Sasuke_WireDash_End : public CState
{
public:
	CState_Sasuke_WireDash_End(const wstring& strStateName, class CPlayer_Sasuke* pPlayer);
	CState_Sasuke_WireDash_End(const CState& rhs, class CStateMachine* pMachine);
	virtual ~CState_Sasuke_WireDash_End() = default;


public:
	// CState��(��) ���� ��ӵ�
	virtual HRESULT Initialize() override;
	virtual void Enter_State() override;
	virtual void Tick_State(_float fTimeDelta) override;
	virtual void Exit_State() override;

	void	Tick_State_Control(_float fTimeDelta);
	void	Tick_State_NoneControl(_float fTimeDelta);
private:
	class CPlayer_Sasuke* m_pPlayer = nullptr;

	std::function<void(CState_Sasuke_WireDash_End&, _float)> m_TickFunc;

private:
	//Animation
	_int m_iAnimIndex = 0;

	Vec3 m_vPrevPos;

public:
	virtual void Free() override;
};

END