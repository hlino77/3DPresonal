#include "stdafx.h"
#include "State_Deidara_Dying_Normal_Server.h"
#include "Boss_Deidara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Dying_Normal_Server::CState_Deidara_Dying_Normal_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Dying_Normal_Server::CState_Deidara_Dying_Normal_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Dying_Normal_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Dying_Type01", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_Dying_Normal_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pBoss->Set_Die();
}

void CState_Deidara_Dying_Normal_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Die");
}

void CState_Deidara_Dying_Normal_Server::Exit_State()
{

}


void CState_Deidara_Dying_Normal_Server::Free()
{
	__super::Free();
}
