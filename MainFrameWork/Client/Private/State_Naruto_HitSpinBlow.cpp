#include "stdafx.h"
#include "State_Naruto_HitSpinBlow.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Naruto_HitSpinBlow::CState_Naruto_HitSpinBlow(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_HitSpinBlow::CState_Naruto_HitSpinBlow(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_HitSpinBlow::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff", 1.2f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_HitSpinBlow::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_HitSpinBlow::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_HitSpinBlow::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 1, 0);
	Knock_Back();
}

void CState_Naruto_HitSpinBlow::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);


	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Fall_Behind");
}

void CState_Naruto_HitSpinBlow::Exit_State()
{

}

void CState_Naruto_HitSpinBlow::Tick_State_Control(_float fTimeDelta)
{
	LookAt_HitObject(fTimeDelta);

	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_HitSpinBlow::Tick_State_NoneControl(_float fTimeDelta)
{
	LookAt_HitObject(fTimeDelta);
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_HitSpinBlow::Knock_Back()
{
	//
	{
		Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		vLook.Normalize();
		Vec3 vUp = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_UP);
		vUp.Normalize();
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);


		m_vHitPos = vPos + vLook + vUp;
	}
	

	//
	//m_vHitPos = m_pPlayer->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
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

	m_pPlayer->Get_RigidBody()->AddForce(vForceDir * 30.0f, ForceMode::FORCE);
}

void CState_Naruto_HitSpinBlow::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_Naruto_HitSpinBlow::Free()
{
	__super::Free();
}
