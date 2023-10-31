#include "stdafx.h"
#include "State_Deidara_HitSpinBlowDown.h"
#include "Boss_Deidara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Deidara_HitSpinBlowDown::CState_Deidara_HitSpinBlowDown(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_HitSpinBlowDown::CState_Deidara_HitSpinBlowDown(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_HitSpinBlowDown::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff_Conect_ToDown", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_HitSpinBlowDown::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
	m_fOriginDrag = m_pBoss->Get_RigidBody()->Get_GruondDrag();
	m_bKnockBack = false;
}

void CState_Deidara_HitSpinBlowDown::Tick_State(_float fTimeDelta)
{
	if (m_bKnockBack == false)
		Knock_Back();


	CModel* pModel = m_pBoss->Get_ModelCom();
	if (pModel == nullptr || pModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	LookAt_HitObject(fTimeDelta);


	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_HitSpinBlowDown::Exit_State()
{
	m_vHitPos = Vec3(0.0f, 0.0f, 0.0f);
	m_pBoss->Get_RigidBody()->Set_GruondDrag(m_fOriginDrag);
}

void CState_Deidara_HitSpinBlowDown::Knock_Back()
{
	m_pBoss->Get_RigidBody()->Set_GruondDrag(1.0f);

	m_vHitPos = m_pBoss->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();

	m_pBoss->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pBoss->Get_RigidBody()->AddForce(vDir * 20.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_Deidara_HitSpinBlowDown::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}


void CState_Deidara_HitSpinBlowDown::Free()
{
	__super::Free();
}
