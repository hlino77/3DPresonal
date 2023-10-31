#include "stdafx.h"
#include "State_Sasuke_HitSpinBlowUp.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Sasuke_HitSpinBlowUp::CState_Sasuke_HitSpinBlowUp(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_HitSpinBlowUp::CState_Sasuke_HitSpinBlowUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_HitSpinBlowUp::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff", 1.4f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_HitSpinBlowUp::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_HitSpinBlowUp::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_HitSpinBlowUp::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bKnockBack = false;
}

void CState_Sasuke_HitSpinBlowUp::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_HitSpinBlowUp::Exit_State()
{

}

void CState_Sasuke_HitSpinBlowUp::Tick_State_Control(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);

	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Fall_Behind");
}

void CState_Sasuke_HitSpinBlowUp::Tick_State_NoneControl(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_HitSpinBlowUp::Knock_Back()
{
	m_vHitPos = m_pPlayer->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	Vec3 vUp = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	_float fLength = vUp.Dot(vDir);
	
	vDir -= (vUp * fLength);

	vDir *= -1.0f;

	vDir.Normalize();

	Vec3 vForceDir = vDir + (vUp * 2.0f);
	vForceDir.Normalize();

	m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pPlayer->Get_RigidBody()->AddForce(vForceDir * 30.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_Sasuke_HitSpinBlowUp::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_Sasuke_HitSpinBlowUp::Free()
{
	__super::Free();
}
