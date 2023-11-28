#include "stdafx.h"
#include "State_Madara_Skill_FireBall_Loop_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Skill_FireBall_Loop_Server::CState_Madara_Skill_FireBall_Loop_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_FireBall_Loop_Server::CState_Madara_Skill_FireBall_Loop_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_FireBall_Loop_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_DestroyerFlame_Loop", 1.0f);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_FireBall_Loop_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Madara_Skill_FireBall_Loop_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Skill_FireBall_End");
}

void CState_Madara_Skill_FireBall_Loop_Server::Exit_State()
{

}


void CState_Madara_Skill_FireBall_Loop_Server::Free()
{
	__super::Free();
}
