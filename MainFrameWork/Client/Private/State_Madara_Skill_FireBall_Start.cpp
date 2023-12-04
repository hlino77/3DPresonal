#include "stdafx.h"
#include "State_Madara_Skill_FireBall_Start.h"
#include "Boss_Madara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_Madara_Skill_FireBall_Start::CState_Madara_Skill_FireBall_Start(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_FireBall_Start::CState_Madara_Skill_FireBall_Start(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_FireBall_Start::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_DestroyerFlame_Start", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_FireBall_Start::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_pBoss->Set_Invincible(true);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"NinjutsuStart.wav", g_fVolume * 0.2f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Madara_Skill_FireBall_Start::Tick_State(_float fTimeDelta)
{
	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_Skill_FireBall_Start::Exit_State()
{
	
}


void CState_Madara_Skill_FireBall_Start::Free()
{
	__super::Free();
}
