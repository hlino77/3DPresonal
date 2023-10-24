#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Naruto_Attack_ElbowStrike.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "ColliderSphere.h"


CState_Naruto_Attack_ElbowStrike::CState_Naruto_Attack_ElbowStrike(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Attack_ElbowStrike::CState_Naruto_Attack_ElbowStrike(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Attack_ElbowStrike::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_ElbowStrike", 2.0f);
	


	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iNextFrame = 51;
	m_iChangeFrame = 17;
	m_iInputNext = m_iNextFrame - 5;
	m_iStartFrame = 0;
	m_iColliderFrame1 = 15;
	m_iColliderFrame2 = 44;


	m_fMoveSpeed = 5.0f;

	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Attack_ElbowStrike::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Attack_ElbowStrike::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_Attack_ElbowStrike::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, m_iStartFrame, m_iChangeFrame);
	m_bSetTargetPos = false;

	if (m_pPlayer->Is_Control())
		m_pPlayer->Set_TargetPos(m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

}

void CState_Naruto_Attack_ElbowStrike::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Attack_ElbowStrike::Exit_State()
{
	if (m_pPlayer->Is_Control())
	{
		CSphereCollider* pCollider = m_pPlayer->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		pCollider->SetActive(false);
		pCollider->Set_AttackType(0);
		m_pPlayer->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	}
}

void CState_Naruto_Attack_ElbowStrike::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;


	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame >= m_iInputNext)
	{
		/*Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);*/
		if (KEY_TAP(KEY::LBTN))
		{
			m_pPlayer->Set_State(L"Attack_Normal_cmb04");
			return;
		}

	}

	if (iFrame >= 4 && iFrame < 32)
	{
		if (!m_bSetTargetPos)
		{
			Set_TargetPos();
			m_bSetTargetPos = true;
		}
		Follow_TargetPos(fTimeDelta);
	}
	
	if (iFrame >= 32 && iFrame < 35)
	{
		m_bSetTargetPos = false;
	}

	if (iFrame >= 35)
	{
		if (!m_bSetTargetPos)
		{
			Set_TargetPos();
			m_bSetTargetPos = true;
		}
		Follow_TargetPos(fTimeDelta);
	}

	if(pPlayerModel->Get_Anim_Frame(m_iAnimIndex) >= m_iColliderFrame1 && pPlayerModel->Get_Anim_Frame(m_iAnimIndex) <= m_iColliderFrame2)
		Update_Collider(fTimeDelta, m_iColliderFrame1);
	else
		Update_Collider(fTimeDelta, m_iColliderFrame2);

	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_Attack_ElbowStrike::Tick_State_NoneControl(_float fTimeDelta)
{
	Follow_TargetPos(fTimeDelta);
}

void CState_Naruto_Attack_ElbowStrike::Update_Collider(_float fTimeDelta, _uint iColliderFrame)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	CSphereCollider* pCollider = m_pPlayer->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame >= iColliderFrame && iFrame < iColliderFrame + 5)
	{
		if (pCollider->IsActive() == false)
		{
			pCollider->SetActive(true);
			pCollider->Set_AttackType((_uint)COLLIDER_ATTACK::MIDDLE);
			pCollider->Set_Attack(1);
			m_pPlayer->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		}
	}

	if (iFrame >= iColliderFrame + 5)
	{
		if (pCollider->IsActive() == true)
		{
			pCollider->SetActive(false);
			pCollider->Set_AttackType(0);
			pCollider->Set_Attack(0);
			m_pPlayer->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		}
	}
}



void CState_Naruto_Attack_ElbowStrike::Follow_TargetPos(_float fTimeDelta)
{
	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();

	if (vPlayerPos == vTargetPos)
		return;

	Vec3 vDir = vTargetPos - vPlayerPos;
	Vec3 vMove = vDir;
	vMove.Normalize();
	vMove *= m_fMoveSpeed * fTimeDelta;

	m_pPlayer->Get_TransformCom()->LookAt_Lerp(vDir, 5.0f, fTimeDelta);

	if (vDir.Length() > vMove.Length())
	{
		vPlayerPos += vMove;
		m_pPlayer->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPlayerPos);
	}
	else
	{
		vPlayerPos = vTargetPos;
		m_pPlayer->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPlayerPos);
	}
}

void CState_Naruto_Attack_ElbowStrike::Set_TargetPos()
{
	CGameObject* pTarget = m_pPlayer->Get_NearTarget();
	if (pTarget)
	{
		Vec3 vTargetObjectPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
		Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

		Vec3 vDistance = vPlayerPos - vTargetObjectPos;

		if (vDistance.Length() <= 3.0f)
		{
			vDistance.Normalize();
			vDistance *= 0.6f;
			Vec3 vTargetPos = vTargetObjectPos + vDistance;
			m_pPlayer->Set_TargetPos(vTargetPos);
			return;
		}
	}

	m_pPlayer->Reset_NearTarget();
	Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

	vLook.Normalize();
	vLook *= 1.0f;

	Vec3 vTargetPos = vPlayerPos + vLook;
	m_pPlayer->Set_TargetPos(vTargetPos);
}

void CState_Naruto_Attack_ElbowStrike::Free()
{
	__super::Free();
}
