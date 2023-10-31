#include "stdafx.h"
#include "State_WhiteZetsu_Die_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Die_Server::CState_WhiteZetsu_Die_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Die_Server::CState_WhiteZetsu_Die_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Die_Server::Initialize()
{


	return S_OK;
}

void CState_WhiteZetsu_Die_Server::Enter_State()
{
	m_pMonster->Set_Die();
}

void CState_WhiteZetsu_Die_Server::Tick_State(_float fTimeDelta)
{
	m_pMonster->Set_Active(false);
}

void CState_WhiteZetsu_Die_Server::Exit_State()
{

}


void CState_WhiteZetsu_Die_Server::Free()
{
	__super::Free();
}
