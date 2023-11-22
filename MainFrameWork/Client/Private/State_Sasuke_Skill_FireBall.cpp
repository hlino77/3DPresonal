#include "stdafx.h"
#include "State_Sasuke_Skill_FireBall.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Skill_FireBall.h"


CState_Sasuke_Skill_FireBall::CState_Sasuke_Skill_FireBall(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Skill_FireBall::CState_Sasuke_Skill_FireBall(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Skill_FireBall::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Fireball_Lv3", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Skill_FireBall::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Skill_FireBall::Tick_State_NoneControl;


	m_iSkillFrame = 15;

	return S_OK;
}

void CState_Sasuke_Skill_FireBall::Enter_State()
{
	if(m_pPlayer->Is_Control())
		Set_Target();

	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bAttack = false;

	m_pPlayer->Set_Invincible(true);
}

void CState_Sasuke_Skill_FireBall::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Skill_FireBall::Exit_State()
{
	m_pPlayer->Set_Invincible(false);
}

void CState_Sasuke_Skill_FireBall::Tick_State_Control(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame <= 15)
	{
		LookAt_Target(fTimeDelta);
	}
	else
	{
		if (m_bAttack == false)
		{
			m_pPlayer->Get_FireBall()->Appear();
			m_bAttack = true;
		}
	}

	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");


}

void CState_Sasuke_Skill_FireBall::Tick_State_NoneControl(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame <= 15)
	{
		LookAt_Target(fTimeDelta);
	}
	else
	{
		if (m_bAttack == false)
		{
			m_pPlayer->Get_FireBall()->Appear();
			m_bAttack = true;
		}
	}


	
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Skill_FireBall::Set_Target()
{
	m_pPlayer->Find_NearTarget();

	CGameObject* pNearTarget = m_pPlayer->Get_NearTarget();

	if (pNearTarget)
	{
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_float fDistance = (vTargetPos - vPos).Length();

		if (fDistance > 15.0f)
		{
			pNearTarget = nullptr;
			m_pPlayer->Set_NearTarget(nullptr);
		}	
	}

	m_pPlayer->Send_NearTarget();

	if (pNearTarget == nullptr)
	{
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		vLook.Normalize();
		Vec3 vTargetPos = vPos + (vLook * 2.0f);
		m_pPlayer->Set_TargetPos(vTargetPos);
	}
}

void CState_Sasuke_Skill_FireBall::LookAt_Target(_float fTimeDelta)
{
	CGameObject* pNearTarget = m_pPlayer->Get_NearTarget();


	Vec3 vTargetPos;
	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	if (pNearTarget == nullptr)
	{
		vTargetPos = m_pPlayer->Get_TargetPos();
	}
	else
	{
		vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	}


	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();

	m_pPlayer->Get_TransformCom()->LookAt_Lerp(vDir, 8.0f, fTimeDelta);
}

void CState_Sasuke_Skill_FireBall::Free()
{
	__super::Free();
}
