#include "stdafx.h"
#include "State_Deidara_Skill_TwinBirds_Server.h"
#include "Boss_Deidara_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Skill_TwinBirds_Server::CState_Deidara_Skill_TwinBirds_Server(const wstring& strStateName, class CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Skill_TwinBirds_Server::CState_Deidara_Skill_TwinBirds_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Skill_TwinBirds_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_TwinBirds", 1.5f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_Skill_TwinBirds_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bTwinBirds = false;
}

void CState_Deidara_Skill_TwinBirds_Server::Tick_State(_float fTimeDelta)
{
	_uint iFrame = m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame >= 20 && iFrame < 37)
	{
		Vec3 vPos = m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = m_pBoss->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		Vec3 vDir = vTargetPos - vPos;
		vDir.Normalize();

		m_pBoss->Get_TransformCom()->LookAt_Lerp(vDir, 7.0f, fTimeDelta);

	}
	else if (m_bTwinBirds == false && iFrame >= 37)
	{
		m_pBoss->Shoot_TwinBirds();
		m_bTwinBirds = true;
	}


	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pBoss->Set_State(L"Idle");
}

void CState_Deidara_Skill_TwinBirds_Server::Exit_State()
{

}


void CState_Deidara_Skill_TwinBirds_Server::Free()
{
	__super::Free();
}
