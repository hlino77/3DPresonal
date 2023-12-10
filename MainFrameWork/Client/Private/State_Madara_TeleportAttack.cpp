#include "stdafx.h"
#include "State_Madara_TeleportAttack.h"
#include "Boss_Madara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_Madara_TeleportAttack::CState_Madara_TeleportAttack(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_TeleportAttack::CState_Madara_TeleportAttack(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_TeleportAttack::Initialize()
{
	m_iIdle_Loop = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_TeleportAttack::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
	m_bMove = false;
	m_pBoss->Set_Render(false);
	m_pBoss->Effect_Teleport();

	Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);

	m_pBoss->Set_Invincible(true);


	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"TeleportAttack.wav", g_fVolume * 0.4f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"Madara_TeleportAttack.wav", g_fVolume * 0.5f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Madara_TeleportAttack::Tick_State(_float fTimeDelta)
{
	
}

void CState_Madara_TeleportAttack::Exit_State()
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
	m_pBoss->Set_Render(true);
	m_pBoss->Effect_Teleport();

	m_pBoss->Set_Invincible(false);
}



void CState_Madara_TeleportAttack::Free()
{
	__super::Free();
}
