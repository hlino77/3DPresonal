#include "stdafx.h"
#include "State_Deidara_HitSpinBlowUp_Server.h"
#include "Boss_Deidara_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Deidara_HitSpinBlowUp_Server::CState_Deidara_HitSpinBlowUp_Server(const wstring& strStateName, CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_HitSpinBlowUp_Server::CState_Deidara_HitSpinBlowUp_Server(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Deidara_HitSpinBlowUp_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff", 1.4f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_HitSpinBlowUp_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bKnockBack = false;
}

void CState_Deidara_HitSpinBlowUp_Server::Tick_State(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);

	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Fall_Behind");
}

void CState_Deidara_HitSpinBlowUp_Server::Exit_State()
{

}


void CState_Deidara_HitSpinBlowUp_Server::Knock_Back()
{
	m_vHitPos = m_pBoss->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	Vec3 vUp = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	_float fLength = vUp.Dot(vDir);
	
	vDir -= (vUp * fLength);

	vDir *= -1.0f;

	vDir.Normalize();

	Vec3 vForceDir = vDir + (vUp * 2.0f);
	vForceDir.Normalize();

	m_pBoss->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pBoss->Get_RigidBody()->AddForce(vForceDir * 30.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_Deidara_HitSpinBlowUp_Server::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_Deidara_HitSpinBlowUp_Server::Free()
{
	__super::Free();
}
