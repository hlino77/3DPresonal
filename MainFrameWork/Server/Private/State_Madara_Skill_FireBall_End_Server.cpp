#include "stdafx.h"
#include "State_Madara_Skill_FireBall_End_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "Skill_MadaraFireBall_Server.h"


CState_Madara_Skill_FireBall_End_Server::CState_Madara_Skill_FireBall_End_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Skill_FireBall_End_Server::CState_Madara_Skill_FireBall_End_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Skill_FireBall_End_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_DestroyerFlame_End", 1.5f);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Skill_FireBall_End_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_bAttack = false;

	Set_Target();
}

void CState_Madara_Skill_FireBall_End_Server::Tick_State(_float fTimeDelta)
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


	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
	{
		m_pBoss->Set_State(L"Idle");
	}
}

void CState_Madara_Skill_FireBall_End_Server::Exit_State()
{
	m_pBoss->Set_FireBall(false);
	m_pBoss->Set_Invincible(false);
}

void CState_Madara_Skill_FireBall_End_Server::Set_Target()
{
	m_pBoss->Find_NearTarget();

	CGameObject* pNearTarget = m_pBoss->Get_NearTarget();

	if (pNearTarget)
	{
		Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_float fDistance = (vTargetPos - vPos).Length();

		if (fDistance > 300.0f)
		{
			pNearTarget = nullptr;
			m_pBoss->Set_NearTarget(nullptr);
		}
	}

	m_pBoss->Send_NearTarget();

	if (pNearTarget == nullptr)
	{
		Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vLook = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		vLook.Normalize();
		Vec3 vTargetPos = vPos + (vLook * 2.0f);
		m_pBoss->Set_TargetPos(vTargetPos);
	}
}

void CState_Madara_Skill_FireBall_End_Server::LookAt_Target(_float fTimeDelta)
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


void CState_Madara_Skill_FireBall_End_Server::Free()
{
	__super::Free();
}
