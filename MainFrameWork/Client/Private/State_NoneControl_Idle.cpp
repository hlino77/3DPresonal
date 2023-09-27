#include "stdafx.h"
#include "State_NoneControl_Idle.h"
#include "Player.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Client_Defines.h"

CState_NoneControl_Idle::CState_NoneControl_Idle(const wstring& strStateName, CPlayer* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_NoneControl_Idle::CState_NoneControl_Idle(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_NoneControl_Idle::Initialize()
{
	return S_OK;
}

void CState_NoneControl_Idle::Enter_State()
{
}

void CState_NoneControl_Idle::Tick_State(_float fTimeDelta)
{
}

void CState_NoneControl_Idle::Exit_State()
{
}

void CState_NoneControl_Idle::Free()
{
	__super::Free();
}
