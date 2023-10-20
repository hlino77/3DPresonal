#include "stdafx.h"
#include "State_WhiteZetsu_Walk_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Walk_Server::CState_WhiteZetsu_Walk_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Walk_Server::CState_WhiteZetsu_Walk_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Walk_Server::Initialize()
{
	m_iWalk_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Walk_Loop", 1.0f);

	if (m_iWalk_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Walk_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iWalk_Loop, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Walk_Server::Tick_State(_float fTimeDelta)
{

}

void CState_WhiteZetsu_Walk_Server::Exit_State()
{

}


void CState_WhiteZetsu_Walk_Server::Free()
{
	__super::Free();
}
