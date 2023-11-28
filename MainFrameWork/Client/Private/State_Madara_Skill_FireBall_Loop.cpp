#include "stdafx.h"
#include "State_Madara_Skill_FireBall_Loop.h"
#include "Boss_Madara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_Madara_Skill_FireBall_Loop::CState_Madara_Skill_FireBall_Loop(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_FireBall_Loop::CState_Madara_Skill_FireBall_Loop(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_FireBall_Loop::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_DestroyerFlame_Loop", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_FireBall_Loop::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Madara_Skill_FireBall_Loop::Tick_State(_float fTimeDelta)
{
	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_Skill_FireBall_Loop::Exit_State()
{

}


void CState_Madara_Skill_FireBall_Loop::Free()
{
	__super::Free();
}
