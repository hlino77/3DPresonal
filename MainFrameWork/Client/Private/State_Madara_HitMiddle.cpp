#include "stdafx.h"
#include "State_Madara_HitMiddle.h"
#include "Boss_Madara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Madara_HitMiddle::CState_Madara_HitMiddle(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_HitMiddle::CState_Madara_HitMiddle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_HitMiddle::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_UpFront_Head", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_HitMiddle::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
	m_bKnockBack = false;
	
}

void CState_Madara_HitMiddle::Tick_State(_float fTimeDelta)
{
	if (m_bKnockBack == false)
		Knock_Back();


	LookAt_HitObject(fTimeDelta);

	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_HitMiddle::Exit_State()
{
	m_vHitPos = Vec3(0.0f, 0.0f, 0.0f);
}

void CState_Madara_HitMiddle::Knock_Back()
{
	m_vHitPos = m_pBoss->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();


	m_pBoss->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pBoss->Get_RigidBody()->AddForce(vDir * 10.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_Madara_HitMiddle::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}


void CState_Madara_HitMiddle::Free()
{
	__super::Free();
}
