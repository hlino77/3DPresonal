#include "stdafx.h"
#include "State_WhiteZetsu_HitSpinBlowDown_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_HitSpinBlowDown_Server::CState_WhiteZetsu_HitSpinBlowDown_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitSpinBlowDown_Server::CState_WhiteZetsu_HitSpinBlowDown_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_HitSpinBlowDown_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_SpinBlowOff_ConectToDown", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitSpinBlowDown_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);


	m_fOriginDrag = m_pMonster->Get_RigidBody()->Get_GruondDrag();


	m_bKnockBack = false;
}

void CState_WhiteZetsu_HitSpinBlowDown_Server::Tick_State(_float fTimeDelta)
{
	if (!m_bKnockBack)
		Knock_Back();

	CModel* pModel = m_pMonster->Get_ModelCom();
	if (pModel == nullptr || pModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	LookAt_HitObject(fTimeDelta);


	if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
	{
		if (m_pMonster->Get_Hp() <= 0)
		{
			m_pMonster->Set_State(L"Die");
		}
		else
			m_pMonster->Set_State(L"GetUp");
	}
}

void CState_WhiteZetsu_HitSpinBlowDown_Server::Exit_State()
{
	m_vHitPos = Vec3(0.0f, 0.0f, 0.0f);
	m_pMonster->Get_RigidBody()->Set_GruondDrag(m_fOriginDrag);
}

void CState_WhiteZetsu_HitSpinBlowDown_Server::Knock_Back()
{
	m_vHitPos = m_pMonster->Get_HitObject()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vOwnerPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_vHitPos.y = vOwnerPos.y;
	Vec3 vDir = vOwnerPos - m_vHitPos;

	vDir.Normalize();

	m_pMonster->Get_RigidBody()->Set_GruondDrag(1.0f);
	m_pMonster->Get_RigidBody()->ClearForce(ForceMode::IMPULSE);
	m_pMonster->Get_RigidBody()->AddForce(vDir * 20.0f, ForceMode::FORCE);

	m_bKnockBack = true;
}

void CState_WhiteZetsu_HitSpinBlowDown_Server::LookAt_HitObject(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_TransformCom();

	Vec3 vOwnerPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = m_vHitPos - vOwnerPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}


void CState_WhiteZetsu_HitSpinBlowDown_Server::Free()
{
	__super::Free();
}
