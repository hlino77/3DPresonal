#include "stdafx.h"
#include "State_Deidara_Die_Server.h"
#include "Boss_Deidara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Die_Server::CState_Deidara_Die_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Die_Server::CState_Deidara_Die_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Die_Server::Initialize()
{


	return S_OK;
}

void CState_Deidara_Die_Server::Enter_State()
{
	m_pBoss->Set_Die();
}

void CState_Deidara_Die_Server::Tick_State(_float fTimeDelta)
{
	m_pBoss->Set_Active(false);
}

void CState_Deidara_Die_Server::Exit_State()
{

}


void CState_Deidara_Die_Server::Free()
{
	__super::Free();
}
