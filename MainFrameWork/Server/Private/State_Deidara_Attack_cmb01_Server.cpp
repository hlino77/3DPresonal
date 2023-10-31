#include "stdafx.h"
#include "State_Deidara_Attack_cmb01_Server.h"
#include "Boss_Deidara_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "ColliderSphere.h"
#include "Server_Defines.h"

CState_Deidara_Attack_cmb01_Server::CState_Deidara_Attack_cmb01_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Attack_cmb01_Server::CState_Deidara_Attack_cmb01_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Attack_cmb01_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb00", 1.2f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iColliderFrame1 = 7;
	m_iColliderFrame2 = 16;

	m_fCollierTime = 0.1f;

	m_iNextFrame = 23;

	return S_OK;
}

void CState_Deidara_Attack_cmb01_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.2f, 2, 0);

	m_pBoss->Set_TargetPos(m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

	m_bSetTargetPos = false;
}

void CState_Deidara_Attack_cmb01_Server::Tick_State(_float fTimeDelta)
{
	CModel* pBossModel = m_pBoss->Get_ModelCom();

	_uint iFrame = pBossModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame >= 2 && iFrame < 9)
	{
		if (!m_bSetTargetPos)
		{
			Set_TargetPos();
			m_bSetTargetPos = true;
		}
		Follow_TargetPos(fTimeDelta);
	}

	if (iFrame >= 9 && iFrame < 12)
	{
		m_bSetTargetPos = false;
	}

	if (iFrame >= 12)
	{
		if (!m_bSetTargetPos)
		{
			Set_TargetPos();
			m_bSetTargetPos = true;
		}
		Follow_TargetPos(fTimeDelta);
	}


	if (iFrame < m_iColliderFrame2 - 2)
		Update_Collider(fTimeDelta, m_iColliderFrame1, COLLIDER_ATTACK::MIDDLE);
	else
		Update_Collider(fTimeDelta, m_iColliderFrame2, COLLIDER_ATTACK::MIDDLE);


	if (iFrame >= m_iNextFrame)
		m_pBoss->Set_State(L"Attack_cmb02");


	if (pBossModel->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Idle");
}

void CState_Deidara_Attack_cmb01_Server::Exit_State()
{
	CSphereCollider* pCollider = m_pBoss->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	pCollider->SetActive(false);
	pCollider->Reset_Attack();
	m_pBoss->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
}

void CState_Deidara_Attack_cmb01_Server::Update_Collider(_float fTimeDelta, _uint iColliderFrame, COLLIDER_ATTACK eAttackType)
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

void CState_Deidara_Attack_cmb01_Server::Follow_TargetPos(_float fTimeDelta)
{
	_float fMoveSpeed = m_pBoss->Get_AttackMoveSpeed();

	if (fMoveSpeed <= 0.0f)
		return;

	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vBossPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pBoss->Get_TargetPos();


	Vec3 vUp = pTransform->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();
	Vec3 vDir = vTargetPos - vBossPos;



	_float fLength = vDir.Dot(vUp);

	vTargetPos = vTargetPos - (vUp * fLength);
	vDir = vTargetPos - vBossPos;

	if (vDir.Length() < 0.001f)
		return;

	Vec3 vMove = vDir;
	vMove.Normalize();
	vMove *= fMoveSpeed * fTimeDelta;

	m_pBoss->Get_TransformCom()->LookAt_Lerp(vDir, 7.0f, fTimeDelta);

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

void CState_Deidara_Attack_cmb01_Server::Set_TargetPos()
{
	CGameObject* pTarget = m_pBoss->Get_NearTarget();
	if (pTarget)
	{
		Vec3 vTargetObjectPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
		Vec3 vBossPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

		Vec3 vDistance = vBossPos - vTargetObjectPos;

		vDistance.Normalize();
		vDistance *= 0.6f;
		Vec3 vTargetPos = vTargetObjectPos + vDistance;
		m_pBoss->Set_TargetPos(vTargetPos);
	}
}


void CState_Deidara_Attack_cmb01_Server::Free()
{
	__super::Free();
}
