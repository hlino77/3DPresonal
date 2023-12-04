#include "stdafx.h"
#include "State_Sasuke_Skill_Chidori_RunLoop.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "LineCircle.h"
#include "RasenganCircle.h"



CState_Sasuke_Skill_Chidori_RunLoop::CState_Sasuke_Skill_Chidori_RunLoop(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Skill_Chidori_RunLoop::CState_Sasuke_Skill_Chidori_RunLoop(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Skill_Chidori_RunLoop::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Chidori_Run_Loop", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Skill_Chidori_RunLoop::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Skill_Chidori_RunLoop::Tick_State_NoneControl;


	return S_OK;
}

void CState_Sasuke_Skill_Chidori_RunLoop::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_fRunTime = 0.7f;
}

void CState_Sasuke_Skill_Chidori_RunLoop::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_RunLoop::Exit_State()
{

}

void CState_Sasuke_Skill_Chidori_RunLoop::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();


	Follow_Target(fTimeDelta);


	if (m_pPlayer->Is_EnemyBodyHit())
	{
		m_pPlayer->Set_State(L"Chidori_Attack");
		return;
	}

	m_fRunTime -= fTimeDelta;

	if (m_fRunTime <= 0.0f)
	{
		m_pPlayer->Set_State(L"Chidori_Attack");
		return;
	}
}

void CState_Sasuke_Skill_Chidori_RunLoop::Tick_State_NoneControl(_float fTimeDelta)
{
	Follow_Target(fTimeDelta);

	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_RunLoop::Free()
{
	__super::Free();
}

void CState_Sasuke_Skill_Chidori_RunLoop::Follow_Target(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	CGameObject* pTarget = m_pPlayer->Get_NearTarget();


	if (pTarget == nullptr)
	{
		Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		pTransform->LookAt_Lerp(vTargetPos, 10.0f, fTimeDelta);
		pTransform->Go_Straight(15.0f, fTimeDelta);
		return;
	}

	Vec3 vTargetPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	vDir.Normalize();
	
	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);

	if (fDistance > 1.4f)
		pTransform->Go_Straight(15.0f, fTimeDelta);
}
