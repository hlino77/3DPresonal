#include "stdafx.h"
#include "State_C2Dragon_Idle.h"
#include "Monster_C2Dragon.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_C2Dragon_Idle::CState_C2Dragon_Idle(const wstring& strStateName, class CMonster_C2Dragon* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_C2Dragon_Idle::CState_C2Dragon_Idle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_C2Dragon_Idle::Initialize()
{
	m_iIdle_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pMonster->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_C2Dragon_Idle::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
	m_pMonster->Reset_NearTarget();
}

void CState_C2Dragon_Idle::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);


	if (m_pMonster->Get_NearTarget())
	{
		CTransform* pTransform = m_pMonster->Get_TransformCom();

		Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = m_pMonster->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vDir = vTargetPos - vPos;

		vDir.Normalize();

		pTransform->LookAt_Lerp(vDir, 3.0f, fTimeDelta);
	}
}

void CState_C2Dragon_Idle::Exit_State()
{
}


void CState_C2Dragon_Idle::Free()
{
	__super::Free();
}
