#include "stdafx.h"
#include "State_Madara_Skill_Meteor_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "Skill_Meteor_Server.h"

CState_Madara_Skill_Meteor_Server::CState_Madara_Skill_Meteor_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_Meteor_Server::CState_Madara_Skill_Meteor_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_Meteor_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_ShatteredHeaven", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_Meteor_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_pBoss->Set_Invincible(true);
}

void CState_Madara_Skill_Meteor_Server::Tick_State(_float fTimeDelta)
{
	_uint iFrame = m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);
	if (iFrame > 70)
	{
		m_pBoss->Get_Meteor()->Appear();
	}



	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
	{
		m_pBoss->Set_State(L"Idle");
	}
}

void CState_Madara_Skill_Meteor_Server::Exit_State()
{
	m_pBoss->Set_TeleportAttackDelay(0.5f);
	m_pBoss->Set_Invincible(false);
}


void CState_Madara_Skill_Meteor_Server::Free()
{
	__super::Free();
}
