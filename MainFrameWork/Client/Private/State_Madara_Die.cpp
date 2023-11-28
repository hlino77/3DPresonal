#include "stdafx.h"
#include "State_Madara_Die.h"
#include "Boss_Madara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Die::CState_Madara_Die(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Die::CState_Madara_Die(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Die::Initialize()
{


	return S_OK;
}

void CState_Madara_Die::Enter_State()
{
	m_pBoss->Set_Die();
	m_pBoss->Effect_Die();
}

void CState_Madara_Die::Tick_State(_float fTimeDelta)
{
	m_pBoss->Set_Active(false);
}

void CState_Madara_Die::Exit_State()
{

}


void CState_Madara_Die::Free()
{
	__super::Free();
}
