#include "stdafx.h"
#include "State_Deidara_HitSpinBlowUp.h"
#include "Boss_Deidara.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Deidara_HitSpinBlowUp::CState_Deidara_HitSpinBlowUp(const wstring& strStateName, CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_HitSpinBlowUp::CState_Deidara_HitSpinBlowUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Deidara_HitSpinBlowUp::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff", 1.4f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_HitSpinBlowUp::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bKnockBack = false;


	m_pBoss->Stop_VoiceSound();
	wstring SoundKey = CGameInstance::GetInstance()->Get_RandomSoundKey(L"Deidara_SpinBlowUp");
	m_pBoss->Set_VoiceSoundKey(SoundKey);
	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(SoundKey, g_fVolume * 0.4f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Deidara_HitSpinBlowUp::Tick_State(_float fTimeDelta)
{
	if(m_bKnockBack == false)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);

	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_HitSpinBlowUp::Exit_State()
{

}


void CState_Deidara_HitSpinBlowUp::Knock_Back()
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

void CState_Deidara_HitSpinBlowUp::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_Deidara_HitSpinBlowUp::Free()
{
	__super::Free();
}
