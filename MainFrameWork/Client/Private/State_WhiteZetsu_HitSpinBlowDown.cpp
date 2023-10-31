#include "stdafx.h"
#include "State_WhiteZetsu_HitSpinBlowDown.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_HitSpinBlowDown::CState_WhiteZetsu_HitSpinBlowDown(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitSpinBlowDown::CState_WhiteZetsu_HitSpinBlowDown(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_HitSpinBlowDown::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff_ConectToDown", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitSpinBlowDown::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);

	m_fOriginDrag = m_pMonster->Get_RigidBody()->Get_GruondDrag();
	m_bKnockBack = false;

}

void CState_WhiteZetsu_HitSpinBlowDown::Tick_State(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	LookAt_HitObject(fTimeDelta);

	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_WhiteZetsu_HitSpinBlowDown::Exit_State()
{
	m_vHitPos = Vec3(0.0f, 0.0f, 0.0f);
	m_pMonster->Get_RigidBody()->Set_GruondDrag(m_fOriginDrag);
}

void CState_WhiteZetsu_HitSpinBlowDown::Knock_Back()
{
	m_vHitPos = m_pMonster->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();

	m_pMonster->Get_RigidBody()->Set_GruondDrag(1.0f);
	m_pMonster->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pMonster->Get_RigidBody()->AddForce(vDir * 20.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_WhiteZetsu_HitSpinBlowDown::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}


void CState_WhiteZetsu_HitSpinBlowDown::Free()
{
	__super::Free();
}
