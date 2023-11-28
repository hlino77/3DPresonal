#include "stdafx.h"
#include "State_Madara_Skill_FireBall_Start_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Skill_FireBall_Start_Server::CState_Madara_Skill_FireBall_Start_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_FireBall_Start_Server::CState_Madara_Skill_FireBall_Start_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_FireBall_Start_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_DestroyerFlame_Start", 1.5f);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_FireBall_Start_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_pBoss->Set_SkillReady(CBoss_Madara_Server::MADARA_SKILL::FIREBALL, false);
	m_pBoss->Set_FireBall(true);
	m_pBoss->Set_Invincible(true);
}

void CState_Madara_Skill_FireBall_Start_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Skill_FireBall_Loop");
}

void CState_Madara_Skill_FireBall_Start_Server::Exit_State()
{
	
}


void CState_Madara_Skill_FireBall_Start_Server::Free()
{
	__super::Free();
}
