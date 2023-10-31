#include "stdafx.h"
#include "State_WhiteZetsu_HitSpinBlowUp.h"
#include "Monster_WhiteZetsu.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_HitSpinBlowUp::CState_WhiteZetsu_HitSpinBlowUp(const wstring& strStateName, CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitSpinBlowUp::CState_WhiteZetsu_HitSpinBlowUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_WhiteZetsu_HitSpinBlowUp::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff", 1.4f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitSpinBlowUp::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bKnockBack = false;
}

void CState_WhiteZetsu_HitSpinBlowUp::Tick_State(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();


	LookAt_HitObject(fTimeDelta);

	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_WhiteZetsu_HitSpinBlowUp::Exit_State()
{

}


void CState_WhiteZetsu_HitSpinBlowUp::Knock_Back()
{
	m_vHitPos = m_pMonster->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	Vec3 vUp = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	_float fLength = vUp.Dot(vDir);
	
	vDir -= (vUp * fLength);

	vDir *= -1.0f;

	vDir.Normalize();

	Vec3 vForceDir = vDir + (vUp * 2.0f);
	vForceDir.Normalize();

	m_pMonster->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pMonster->Get_RigidBody()->AddForce(vForceDir * 30.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_WhiteZetsu_HitSpinBlowUp::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_WhiteZetsu_HitSpinBlowUp::Free()
{
	__super::Free();
}
