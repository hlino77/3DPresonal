#include "stdafx.h"
#include "State_C2Dragon_Attack_Server.h"
#include "Monster_C2Dragon_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_C2Dragon_Attack_Server::CState_C2Dragon_Attack_Server(const wstring& strStateName, class CMonster_C2Dragon_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_C2Dragon_Attack_Server::CState_C2Dragon_Attack_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_C2Dragon_Attack_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;


	return S_OK;
}

void CState_C2Dragon_Attack_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bShoot = false;
}

void CState_C2Dragon_Attack_Server::Tick_State(_float fTimeDelta)
{
	
	if (m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex) < 20)
	{
		CTransform* pTransform = m_pMonster->Get_TransformCom();

		Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = m_pMonster->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vDir = vTargetPos - vPos;

		vDir.Normalize();

		pTransform->LookAt_Lerp(vDir, 3.0f, fTimeDelta);
	}

	if (m_bShoot == false && m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex) >= 23)
	{
		m_pMonster->Shoot_Hiryu();
		m_bShoot = true;
	}


	if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pMonster->Set_State(L"Idle");
}

void CState_C2Dragon_Attack_Server::Exit_State()
{

}



void CState_C2Dragon_Attack_Server::Free()
{
	__super::Free();
}
