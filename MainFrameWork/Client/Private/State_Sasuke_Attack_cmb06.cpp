#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Sasuke_Attack_cmb06.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "ColliderSphere.h"

CState_Sasuke_Attack_cmb06::CState_Sasuke_Attack_cmb06(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Attack_cmb06::CState_Sasuke_Attack_cmb06(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Attack_cmb06::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb06", 1.0f);
	


	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iNextFrame = 26;
	m_iChangeFrame = 16;
	m_iInputNext = 0;
	m_iStartFrame = 4;
	m_iColliderFrame = 23;

	m_fMoveSpeed = 5.0f;

	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Attack_cmb06::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Attack_cmb06::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_Attack_cmb06::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, m_iStartFrame, m_iChangeFrame);
	if (m_pPlayer->Is_Control())
	{
		if (m_pPlayer->Get_NearTarget() == nullptr)
			Set_TargetPos();
	}
}

void CState_Sasuke_Attack_cmb06::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Attack_cmb06::Exit_State()
{
	if (m_pPlayer->Is_Control())
	{
		CSphereCollider* pCollider = m_pPlayer->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		pCollider->SetActive(false);
		pCollider->Set_AttackType(0);
		m_pPlayer->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	}
}

void CState_Sasuke_Attack_cmb06::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	if (m_pPlayer->Get_NearTarget() == nullptr)
		Follow_TargetPos(fTimeDelta);
	else
		Follow_TargetObject(fTimeDelta);


	Update_Collider(fTimeDelta);

	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Sasuke_Attack_cmb06::Tick_State_NoneControl(_float fTimeDelta)
{
	if (m_pPlayer->Get_NearTarget() == nullptr)
		Follow_TargetPos(fTimeDelta);
	else
		Follow_TargetObject(fTimeDelta);
}

void CState_Sasuke_Attack_cmb06::Update_Collider(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	CSphereCollider* pCollider = m_pPlayer->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	if (pPlayerModel->Get_Anim_Frame(m_iAnimIndex) >= m_iColliderFrame)
	{
		if (pCollider->IsActive() == false)
		{
			pCollider->SetActive(true);
			pCollider->Set_AttackType((_uint)COLLIDER_ATTACK::MIDDLE);
			m_pPlayer->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		}
	}

	if (pPlayerModel->Get_Anim_Frame(m_iAnimIndex) > m_iNextFrame)
	{
		if (pCollider->IsActive() == true)
		{
			pCollider->SetActive(false);
			pCollider->Set_AttackType(0);
			m_pPlayer->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		}
	}
}

void CState_Sasuke_Attack_cmb06::Follow_TargetObject(_float fTimeDelta)
{
	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetObjectPos = m_pPlayer->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);


	Vec3 vDir = vTargetObjectPos - vPlayerPos;
	vDir.Normalize();

	Vec3 vTargetPos = vTargetObjectPos + vDir * -0.8f;
	Vec3 vTargetDistance = vTargetPos - vPlayerPos;
	Vec3 vMove = vDir;
	vMove.Normalize();
	vMove *= m_fMoveSpeed * fTimeDelta;

	m_pPlayer->Get_TransformCom()->LookAt_Lerp(vDir, 5.0f, fTimeDelta);

	if (vTargetDistance.Length() > vMove.Length())
	{
		vPlayerPos += vMove;
		m_pPlayer->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPlayerPos);
	}
	else
		vPlayerPos = vTargetPos;
}

void CState_Sasuke_Attack_cmb06::Follow_TargetPos(_float fTimeDelta)
{
	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();

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
		vPlayerPos = vTargetPos;
}

void CState_Sasuke_Attack_cmb06::Set_TargetPos()
{
	Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

	vLook.Normalize();
	vLook *= 1.0f;

	Vec3 vTargetPos = vPlayerPos + vLook;
	m_pPlayer->Set_TargetPos(vTargetPos);
}

void CState_Sasuke_Attack_cmb06::Free()
{
	__super::Free();
}
