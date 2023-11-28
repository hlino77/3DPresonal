#include "stdafx.h"
#include "State_Madara_Idle_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Idle_Server::CState_Madara_Idle_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Idle_Server::CState_Madara_Idle_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Idle_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Idle_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_pBoss->Reset_FindTargetTime();
}

void CState_Madara_Idle_Server::Tick_State(_float fTimeDelta)
{
	m_pBoss->Update_NearTarget(fTimeDelta);

	if (m_pBoss->Get_SkillReady(CBoss_Madara_Server::MADARA_SKILL::FIREBALL))
		m_pBoss->Set_State(L"Skill_FireBall_Start");


	m_pBoss->Ready_TeleportAttack(fTimeDelta);
}

void CState_Madara_Idle_Server::Exit_State()
{

}


void CState_Madara_Idle_Server::Free()
{
	__super::Free();
}
