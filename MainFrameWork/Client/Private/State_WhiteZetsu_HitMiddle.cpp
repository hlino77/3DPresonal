#include "stdafx.h"
#include "State_WhiteZetsu_HitMiddle.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_HitMiddle::CState_WhiteZetsu_HitMiddle(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitMiddle::CState_WhiteZetsu_HitMiddle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_HitMiddle::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_UpFront_Head", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitMiddle::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
	Knock_Back();
}

void CState_WhiteZetsu_HitMiddle::Tick_State(_float fTimeDelta)
{
	LookAt_HitObject(fTimeDelta);

	m_pMonster->Follow_ServerPos(0.05f, 0.05f);
}

void CState_WhiteZetsu_HitMiddle::Exit_State()
{
	m_vHitPos = Vec3(0.0f, 0.0f, 0.0f);
}

void CState_WhiteZetsu_HitMiddle::Knock_Back()
{
	m_vHitPos = m_pMonster->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();

	m_pMonster->Get_RigidBody()->AddForce(vDir * 300.f, ForceMode::FORCE);
}

void CState_WhiteZetsu_HitMiddle::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}


void CState_WhiteZetsu_HitMiddle::Free()
{
	__super::Free();
}
