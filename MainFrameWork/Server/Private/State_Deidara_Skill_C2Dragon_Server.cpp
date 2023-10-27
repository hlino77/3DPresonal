#include "stdafx.h"
#include "State_Deidara_Skill_C2Dragon_Server.h"
#include "Boss_Deidara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Skill_C2Dragon_Server::CState_Deidara_Skill_C2Dragon_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Skill_C2Dragon_Server::CState_Deidara_Skill_C2Dragon_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Skill_C2Dragon_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_C2Dragon", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_Skill_C2Dragon_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bDragon = false;
}

void CState_Deidara_Skill_C2Dragon_Server::Tick_State(_float fTimeDelta)
{
	if (m_bDragon == false && m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex) > 15)
	{
		m_pBoss->Spawn_C2Dragon();
		m_bDragon = true;
	}


	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
	{
		m_pBoss->Set_State(L"Idle");
	}
}

void CState_Deidara_Skill_C2Dragon_Server::Exit_State()
{

}


void CState_Deidara_Skill_C2Dragon_Server::Free()
{
	__super::Free();
}
