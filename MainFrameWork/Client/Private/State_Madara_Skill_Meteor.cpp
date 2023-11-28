#include "stdafx.h"
#include "State_Madara_Skill_Meteor.h"
#include "Boss_Madara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "Monster_Susanoo_Madara.h"
#include "Skill_Meteor.h"

CState_Madara_Skill_Meteor::CState_Madara_Skill_Meteor(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_Meteor::CState_Madara_Skill_Meteor(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_Meteor::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_ShatteredHeaven", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;


	CGameObject* pSusanoo = CGameInstance::GetInstance()->Add_GameObject(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_EFFECT, L"Prototype_GameObject_Monster_Susanoo_Madara");
	m_pSusanoo = dynamic_cast<CMonster_Susanoo_Madara*>(pSusanoo);
	if (m_pSusanoo == nullptr)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_Meteor::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pSusanoo->Appear(m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_LOOK));

	m_pBoss->Set_Invincible(true);
}

void CState_Madara_Skill_Meteor::Tick_State(_float fTimeDelta)
{
	_uint iFrame = m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);
	if (iFrame > 70)
	{
		m_pBoss->Get_Meteor()->Appear();
	}


	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_Skill_Meteor::Exit_State()
{
	m_pBoss->Set_Invincible(false);
}


void CState_Madara_Skill_Meteor::Free()
{
	__super::Free();
}
