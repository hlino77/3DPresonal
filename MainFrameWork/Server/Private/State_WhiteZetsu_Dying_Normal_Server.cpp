#include "stdafx.h"
#include "State_WhiteZetsu_Dying_Normal_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Dying_Normal_Server::CState_WhiteZetsu_Dying_Normal_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Dying_Normal_Server::CState_WhiteZetsu_Dying_Normal_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Dying_Normal_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Dying_Type01", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Dying_Normal_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_pMonster->Set_Die();
}

void CState_WhiteZetsu_Dying_Normal_Server::Tick_State(_float fTimeDelta)
{
	if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pMonster->Set_Active(false);
}

void CState_WhiteZetsu_Dying_Normal_Server::Exit_State()
{

}


void CState_WhiteZetsu_Dying_Normal_Server::Free()
{
	__super::Free();
}
