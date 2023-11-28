#include "stdafx.h"
#include "State_Madara_Skill_FireBall_End.h"
#include "Boss_Madara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "Skill_MadaraFireBall.h"


CState_Madara_Skill_FireBall_End::CState_Madara_Skill_FireBall_End(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_FireBall_End::CState_Madara_Skill_FireBall_End(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_FireBall_End::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_DestroyerFlame_End", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_FireBall_End::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_bAttack = false;
}

void CState_Madara_Skill_FireBall_End::Tick_State(_float fTimeDelta)
{
	_uint iFrame = m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame <= 32)
	{
		LookAt_Target(fTimeDelta);
	}
	else
	{
		if (m_bAttack == false)
		{
			m_pBoss->Get_FireBall()->Appear();
			m_bAttack = true;
		}
	}


	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_Skill_FireBall_End::Exit_State()
{
	m_pBoss->Set_Invincible(false);
}

void CState_Madara_Skill_FireBall_End::LookAt_Target(_float fTimeDelta)
{
	CGameObject* pNearTarget = m_pBoss->Get_NearTarget();


	Vec3 vTargetPos;
	Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	if (pNearTarget == nullptr)
	{
		vTargetPos = m_pBoss->Get_TargetPos();
	}
	else
	{
		vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	}


	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();

	m_pBoss->Get_TransformCom()->LookAt_Lerp(vDir, 8.0f, fTimeDelta);
}


void CState_Madara_Skill_FireBall_End::Free()
{
	__super::Free();
}
