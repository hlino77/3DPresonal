#include "stdafx.h"
#include "State_Madara_TeleportAttack_Server.h"
#include "Boss_Madara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_TeleportAttack_Server::CState_Madara_TeleportAttack_Server(const wstring& strStateName, class CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_TeleportAttack_Server::CState_Madara_TeleportAttack_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_TeleportAttack_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_TeleportAttack_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bMove = false;

	m_fDelay = 0.5f;

	Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);


	m_pBoss->Set_Invincible(true);
}

void CState_Madara_TeleportAttack_Server::Tick_State(_float fTimeDelta)
{
	m_fDelay -= fTimeDelta;

	if (m_fDelay <= 0.0f)
	{
		m_pBoss->Set_State(L"Attack_Punch");
	}
}

void CState_Madara_TeleportAttack_Server::Exit_State()
{
	Vec3 vPlayerPos = m_pBoss->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vPlayerPos.y = vPos.y;

	Vec3 vDir = vPos - vPlayerPos;
	vDir.Normalize();

	vDir *= 1.0f;
	Vec3 vTargetPos = vPlayerPos + vDir;


	m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vTargetPos);
	m_pBoss->Get_TransformCom()->LookAt_ForLandObject(vPlayerPos);

	m_pBoss->Set_Invincible(false);
}


void CState_Madara_TeleportAttack_Server::Free()
{
	__super::Free();
}
