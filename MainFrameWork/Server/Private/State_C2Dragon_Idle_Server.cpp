#include "stdafx.h"
#include "State_C2Dragon_Idle_Server.h"
#include "Monster_C2Dragon_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "Skill_Hiryu_Server.h"

CState_C2Dragon_Idle_Server::CState_C2Dragon_Idle_Server(const wstring& strStateName, class CMonster_C2Dragon_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_C2Dragon_Idle_Server::CState_C2Dragon_Idle_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_C2Dragon_Idle_Server::Initialize()
{
	m_iIdle_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);

	if (m_iIdle_Loop == -1)
		return E_FAIL;


	m_fAttackDelay = 5.0f;

	return S_OK;
}

void CState_C2Dragon_Idle_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
	m_pMonster->Set_NearTarget(nullptr);
	m_bUpdateTarget = false;

	m_fCurrAttackTime = 0.0f;
}

void CState_C2Dragon_Idle_Server::Tick_State(_float fTimeDelta)
{
	Update_Attack(fTimeDelta);
}

void CState_C2Dragon_Idle_Server::Exit_State()
{

}

void CState_C2Dragon_Idle_Server::Update_Attack(_float fTimeDelta)
{
	m_fCurrAttackTime += fTimeDelta;
	_float fAttackDelay = m_pMonster->Get_AttackDelay();
	
	if (m_pMonster->Get_Hiryu()->Is_Active())
		return;

	if (m_fCurrAttackTime >= fAttackDelay)
	{
		if (!m_bUpdateTarget)
		{
			m_pMonster->Update_NearTarget(fTimeDelta);

			if (m_pMonster->Get_NearTarget())
			{
				m_bUpdateTarget = true;
				m_pMonster->Send_NearTarget();
			}
			else
				return;
		}
	

		CTransform* pTransform = m_pMonster->Get_TransformCom();

		Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		Vec3 vLook = pTransform->Get_State(CTransform::STATE_LOOK);
		Vec3 vTargetPos = m_pMonster->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vTargetPos.y = vPos.y;
		Vec3 vDir = vTargetPos - vPos;
		

		vLook.Normalize();
		vDir.Normalize();

		_float fAngle = XMConvertToDegrees(acosf(vLook.Dot(vDir)));
		if (fAngle < 30.0f)
		{
			m_pMonster->Set_State(L"Attack");
			m_pMonster->Set_AttackDelay(m_fAttackDelay);
		}
		else
		{
			_float fDelay = m_fCurrAttackTime - fAttackDelay;
			if (fDelay > 2.0f)
			{
				m_pMonster->Set_State(L"Attack");
				m_pMonster->Set_AttackDelay(m_fAttackDelay);
			}
			else
				pTransform->LookAt_Lerp(vDir, 3.0f, fTimeDelta);
		}
	}
}


void CState_C2Dragon_Idle_Server::Free()
{
	__super::Free();
}
