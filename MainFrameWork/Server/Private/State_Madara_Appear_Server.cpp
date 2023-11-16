#include "stdafx.h"
#include "State_Madara_Appear_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Appear_Server::CState_Madara_Appear_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Appear_Server::CState_Madara_Appear_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Appear_Server::Initialize()
{
	m_iAppear = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Appear", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAppear);
	if (m_iAppear == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Appear_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAppear, 0.1f, 0, 0);
}

void CState_Madara_Appear_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAppear))
		m_pBoss->Set_State(L"Idle");
}

void CState_Madara_Appear_Server::Exit_State()
{

}


void CState_Madara_Appear_Server::Free()
{
	__super::Free();
}
