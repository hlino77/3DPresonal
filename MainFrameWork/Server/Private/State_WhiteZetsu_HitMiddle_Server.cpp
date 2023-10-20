#include "stdafx.h"
#include "State_WhiteZetsu_HitMiddle_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_HitMiddle_Server::CState_WhiteZetsu_HitMiddle_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitMiddle_Server::CState_WhiteZetsu_HitMiddle_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_HitMiddle_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_UpFront_Head", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitMiddle_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
	Knock_Back();
}

void CState_WhiteZetsu_HitMiddle_Server::Tick_State(_float fTimeDelta)
{
	CModel* pModel = m_pMonster->Get_ModelCom();
	if (pModel == nullptr || pModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	LookAt_HitObject(fTimeDelta);


	if (pModel->Is_AnimationEnd(m_iAnimIndex))
		m_pMonster->Set_State(L"Idle");
}

void CState_WhiteZetsu_HitMiddle_Server::Exit_State()
{
	m_vHitPos = Vec3(0.0f, 0.0f, 0.0f);
}

void CState_WhiteZetsu_HitMiddle_Server::Knock_Back()
{
	m_vHitPos = m_pMonster->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();

	m_pMonster->Get_RigidBody()->AddForce(vDir * 6.f, ForceMode::FORCE);
}

void CState_WhiteZetsu_HitMiddle_Server::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}


void CState_WhiteZetsu_HitMiddle_Server::Free()
{
	__super::Free();
}
