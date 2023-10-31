#include "stdafx.h"
#include "State_WhiteZetsu_FallBehind_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_FallBehind_Server::CState_WhiteZetsu_FallBehind_Server(const wstring& strStateName, CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_FallBehind_Server::CState_WhiteZetsu_FallBehind_Server(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_WhiteZetsu_FallBehind_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Fall_Behind_Loop", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_FallBehind_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_WhiteZetsu_FallBehind_Server::Tick_State(_float fTimeDelta)
{
	if (m_pMonster->Get_RigidBody()->Is_Gruond())
		m_pMonster->Set_State(L"DownToFloor");
}

void CState_WhiteZetsu_FallBehind_Server::Exit_State()
{

}

void CState_WhiteZetsu_FallBehind_Server::Free()
{
	__super::Free();
}
