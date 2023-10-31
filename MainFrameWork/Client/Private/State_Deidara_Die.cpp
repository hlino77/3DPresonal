#include "stdafx.h"
#include "State_Deidara_Die.h"
#include "Boss_Deidara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Die::CState_Deidara_Die(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Die::CState_Deidara_Die(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Die::Initialize()
{


	return S_OK;
}

void CState_Deidara_Die::Enter_State()
{
	m_pBoss->Set_Die();
}

void CState_Deidara_Die::Tick_State(_float fTimeDelta)
{
	m_pBoss->Set_Active(false);
}

void CState_Deidara_Die::Exit_State()
{

}


void CState_Deidara_Die::Free()
{
	__super::Free();
}
