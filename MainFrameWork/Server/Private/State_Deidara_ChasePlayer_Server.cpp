#include "stdafx.h"
#include "State_Deidara_ChasePlayer_Server.h"
#include "Boss_Deidara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_ChasePlayer_Server::CState_Deidara_ChasePlayer_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_ChasePlayer_Server::CState_Deidara_ChasePlayer_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_ChasePlayer_Server::Initialize()
{
	m_iRun_Loop = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 1.0f);


	if (m_iRun_Loop == -1)
		return E_FAIL;


	m_fAccel = 10.0f;
	m_fMaxSpeed = 6.0f;

	return S_OK;
}

void CState_Deidara_ChasePlayer_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iRun_Loop, 0.1f, 0, 0);
}

void CState_Deidara_ChasePlayer_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_SkillReady(CBoss_Deidara_Server::DEIDARA_SKILL::C2DRAGON))
	{
		m_pBoss->Set_State(L"Skill_C2Dragon");
		return;
	}

	m_pBoss->Update_NearTarget(fTimeDelta);

	

	CTransform* pTransform = m_pBoss->Get_TransformCom();
	Vec3 vTargetPos = m_pBoss->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);

	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	
	vDir.Normalize();

	if (fDistance > m_pBoss->Get_FollowDistance())
		m_pBoss->Set_State(L"Idle");
	/*else if (fDistance <= 3.0f)
	{	
		if(m_pBoss->Get_CurrState() != L"Attack_cmb01")
			m_pBoss->Set_State(L"Attack_cmb01");
	}
	else if (fDistance <= 10.0f && m_pBoss->Get_SkillReady(CBoss_Deidara_Server::DEIDARA_SKILL::TWINBIRD))
		m_pBoss->Set_State(L"Skill_TwinBirds");*/
	else
	{
		_float fCurrSpeed = m_pBoss->Get_MoveSpeed();

		if (fCurrSpeed < m_fMaxSpeed)
		{
			fCurrSpeed += m_fAccel * fTimeDelta;
			fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
			m_pBoss->Set_MoveSpeed(fCurrSpeed);
		}
		m_pBoss->Move_Dir(vDir, fCurrSpeed, fTimeDelta);
	}

}

void CState_Deidara_ChasePlayer_Server::Exit_State()
{
	m_pBoss->Set_MoveSpeed(0.0f);
}


void CState_Deidara_ChasePlayer_Server::Free()
{
	__super::Free();
}
