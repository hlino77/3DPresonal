#include "stdafx.h"
#include "State_Madara_Attack_DoubleTurnKick_Server.h"
#include "Boss_Madara_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "ColliderSphere.h"
#include "Server_Defines.h"

CState_Madara_Attack_DoubleTurnKick_Server::CState_Madara_Attack_DoubleTurnKick_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Attack_DoubleTurnKick_Server::CState_Madara_Attack_DoubleTurnKick_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Attack_DoubleTurnKick_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Attack_DoubleTurnKick", 2.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iColliderFrame1 = 7;
	m_iColliderFrame2 = 13;

	m_fCollierTime = 0.1f;


	return S_OK;
}

void CState_Madara_Attack_DoubleTurnKick_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.2f, 0, 0);

	m_pBoss->Set_TargetPos(m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	m_bSetTargetPos = false;
}

void CState_Madara_Attack_DoubleTurnKick_Server::Tick_State(_float fTimeDelta)
{
	CModel* pBossModel = m_pBoss->Get_ModelCom();

	_uint iFrame = pBossModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame >= 0 && iFrame < 7)
	{
		Follow_TargetPos(fTimeDelta);
	}


	if (iFrame >= 8 && iFrame < 13)
	{
		Follow_TargetPos(fTimeDelta);
	}


	if (iFrame < m_iColliderFrame2 - 2)
		Update_Collider(fTimeDelta, m_iColliderFrame1, COLLIDER_ATTACK::MIDDLE);
	else
		Update_Collider(fTimeDelta, m_iColliderFrame2, COLLIDER_ATTACK::SPINBLOWUP);


	if (pBossModel->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Idle");
}

void CState_Madara_Attack_DoubleTurnKick_Server::Exit_State()
{
	CSphereCollider* pCollider = m_pBoss->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	pCollider->SetActive(false);
	pCollider->Reset_Attack();
	m_pBoss->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
}

void CState_Madara_Attack_DoubleTurnKick_Server::Update_Collider(_float fTimeDelta, _uint iColliderFrame, COLLIDER_ATTACK eAttackType)
{
	CModel* pBossModel = m_pBoss->Get_ModelCom();

	CSphereCollider* pCollider = m_pBoss->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	_uint iFrame = pBossModel->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame < iColliderFrame)
	{
		m_bAttack = false;
		return;
	}


	if (m_bAttack == false)
	{
		if (iFrame >= iColliderFrame)
		{
			if (pCollider->IsActive() == false)
			{
				pCollider->SetActive(true);
				pCollider->Set_AttackCollider(1, (_uint)eAttackType, true);
				m_pBoss->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

				m_fCurrTime = 0.0f;
				m_bAttack = true;
			}
		}
	}
	else
	{
		if (pCollider->IsActive() == true)
		{
			m_fCurrTime += fTimeDelta;

			if (m_fCurrTime >= m_fCollierTime)
			{
				pCollider->SetActive(false);
				pCollider->Reset_Attack();
				m_pBoss->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
		}
	}
}

void CState_Madara_Attack_DoubleTurnKick_Server::Follow_TargetPos(_float fTimeDelta)
{
	_float fMoveSpeed = m_pBoss->Get_AttackMoveSpeed();

	if (fMoveSpeed <= 0.0f)
		return;

	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vBossPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pBoss->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);


	vTargetPos.y = vBossPos.y;

	Vec3 vDir = vTargetPos - vBossPos;
	vDir.Normalize();

	m_pBoss->Get_TransformCom()->LookAt_Lerp(vDir, 10.0f, fTimeDelta);

	vTargetPos = vTargetPos + (vDir * -0.6f);
	vDir = vTargetPos - vBossPos;

	if (vDir.Length() < 0.001f)
		return;

	Vec3 vMove = vDir;
	vMove.Normalize();
	vMove *= fMoveSpeed * fTimeDelta;

	if (vDir.Length() > vMove.Length())
	{
		vBossPos += vMove;
		m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vBossPos);
	}
	else
	{
		vBossPos = vTargetPos;
		m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vBossPos);
	}
}


void CState_Madara_Attack_DoubleTurnKick_Server::Free()
{
	__super::Free();
}
