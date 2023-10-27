#include "stdafx.h"
#include "State_C2Dragon_Attack.h"
#include "Monster_C2Dragon.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_C2Dragon_Attack::CState_C2Dragon_Attack(const wstring& strStateName, class CMonster_C2Dragon* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_C2Dragon_Attack::CState_C2Dragon_Attack(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_C2Dragon_Attack::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack", 1.0f);
	m_pMonster->Get_ModelCom()->Set_CurrAnim(m_iAnimIndex);
	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_C2Dragon_Attack::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bShoot = false;
}

void CState_C2Dragon_Attack::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);

	if (m_pMonster->Get_NearTarget() && m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex) < 20)
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

}

void CState_C2Dragon_Attack::Exit_State()
{

}


void CState_C2Dragon_Attack::Free()
{
	__super::Free();
}
