#include "stdafx.h"
#include "State_WhiteZetsu_WalkEnd_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_WalkEnd_Server::CState_WhiteZetsu_WalkEnd_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_WalkEnd_Server::CState_WhiteZetsu_WalkEnd_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_WalkEnd_Server::Initialize()
{
	m_iWalk_End = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Walk_End", 1.0f);

	if (m_iWalk_End == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_WalkEnd_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iWalk_End, 0.1f, 0, 0);
}

void CState_WhiteZetsu_WalkEnd_Server::Tick_State(_float fTimeDelta)
{

}

void CState_WhiteZetsu_WalkEnd_Server::Exit_State()
{

}


void CState_WhiteZetsu_WalkEnd_Server::Free()
{
	__super::Free();
}
