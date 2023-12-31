#include "stdafx.h"
#include "State_Naruto_HitMiddle.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "WireTrail.h"

CState_Naruto_HitMiddle::CState_Naruto_HitMiddle(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_HitMiddle::CState_Naruto_HitMiddle(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_HitMiddle::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_UpFront_Head", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_HitMiddle::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_HitMiddle::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_HitMiddle::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 1, 0);
	m_bKnockBack = false;


	m_pPlayer->Get_WireTrail()->Set_Active(false);


	if (m_pPlayer->Stop_VoiceSound() == true)
	{
		wstring SoundKey = CGameInstance::GetInstance()->Get_RandomSoundKey(L"Naruto_HitMiddle");
		m_pPlayer->Set_VoiceSoundKey(SoundKey, 0.5f);
		CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(SoundKey, g_fVolume * 0.4f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
	}
}

void CState_Naruto_HitMiddle::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_HitMiddle::Exit_State()
{

}

void CState_Naruto_HitMiddle::Tick_State_Control(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);

	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_HitMiddle::Tick_State_NoneControl(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_HitMiddle::Knock_Back()
{
	m_vHitPos = m_pPlayer->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();

	m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pPlayer->Get_RigidBody()->AddForce(vDir * 10.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_Naruto_HitMiddle::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}

void CState_Naruto_HitMiddle::Free()
{
	__super::Free();
}
