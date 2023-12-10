#include "stdafx.h"
#include "State_Naruto_HitSpinBlowUp.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "WireTrail.h"

CState_Naruto_HitSpinBlowUp::CState_Naruto_HitSpinBlowUp(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_HitSpinBlowUp::CState_Naruto_HitSpinBlowUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_HitSpinBlowUp::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff", 1.4f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_HitSpinBlowUp::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_HitSpinBlowUp::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_HitSpinBlowUp::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bKnockBack = false;
	m_pPlayer->Get_TransformCom()->Set_Up(Vec3(0.0f, 1.0f, 0.0f));

	m_pPlayer->Set_Gravity(true);
	//m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Get_RigidBody()->UseGravity(true);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(true);
	m_pPlayer->Set_Wall(false);
	m_pPlayer->DisAppear_FootTrail();

	m_pPlayer->Get_WireTrail()->Set_Active(false);


	m_pPlayer->Stop_VoiceSound();
	wstring SoundKey = CGameInstance::GetInstance()->Get_RandomSoundKey(L"Naruto_SpinBlowUp");
	m_pPlayer->Set_VoiceSoundKey(SoundKey);
	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(SoundKey, g_fVolume * 0.4f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Naruto_HitSpinBlowUp::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_HitSpinBlowUp::Exit_State()
{
	m_bSound = false;
}

void CState_Naruto_HitSpinBlowUp::Tick_State_Control(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();


	LookAt_HitObject(fTimeDelta);

	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Fall_Behind");
}

void CState_Naruto_HitSpinBlowUp::Tick_State_NoneControl(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);

}

void CState_Naruto_HitSpinBlowUp::Knock_Back()
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

void CState_Naruto_HitSpinBlowUp::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_Naruto_HitSpinBlowUp::Free()
{
	__super::Free();
}
