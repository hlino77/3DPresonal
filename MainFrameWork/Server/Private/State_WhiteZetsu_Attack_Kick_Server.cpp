#include "stdafx.h"
#include "State_WhiteZetsu_Attack_Kick_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "ColliderSphere.h"

CState_WhiteZetsu_Attack_Kick_Server::CState_WhiteZetsu_Attack_Kick_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Attack_Kick_Server::CState_WhiteZetsu_Attack_Kick_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Attack_Kick_Server::Initialize()
{
	m_iAttack_JumpTurnKick = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack_JumpTurnKick", 1.0f);

	if (m_iAttack_JumpTurnKick == -1)
		return E_FAIL;

	m_iColliderFrame1 = 17;
	m_iColliderFrame2 = 29;


	m_iNextFrame = 59;

	return S_OK;
}

void CState_WhiteZetsu_Attack_Kick_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAttack_JumpTurnKick, 0.3f, 0, 0);
}

void CState_WhiteZetsu_Attack_Kick_Server::Tick_State(_float fTimeDelta)
{
	Update_Collider();

	_uint iFrame = m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAttack_JumpTurnKick);
	if (iFrame >= m_iNextFrame)
	{
		Vec3 vTargetPos = m_pMonster->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		Vec3 vPos = m_pMonster->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_float fDistance = (vTargetPos - vPos).Length();

		if (fDistance < 1.1f)
			m_pMonster->Set_State(L"Attack_Punch");
		else
			m_pMonster->Set_State(L"Idle");
	}
	

}

void CState_WhiteZetsu_Attack_Kick_Server::Exit_State()
{
	CSphereCollider* pCollider = m_pMonster->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	pCollider->SetActive(false);
	m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
}

void CState_WhiteZetsu_Attack_Kick_Server::Update_Collider()
{
	_uint iFrame = m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAttack_JumpTurnKick);
	CSphereCollider* pCollider = m_pMonster->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	if (iFrame <= m_iColliderFrame1 + 3)
	{
		if (iFrame >= m_iColliderFrame1 && iFrame < m_iColliderFrame1 + 3)
		{
			if (pCollider->IsActive() == false)
			{
				pCollider->SetActive(true);
				pCollider->Set_AttackType((_uint)COLLIDER_ATTACK::MIDDLE);
				m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
		}


		if (iFrame >= m_iColliderFrame1 + 3)
		{
			if (pCollider->IsActive())
			{
				pCollider->SetActive(false);
				m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
		}
	}
	else
	{

		if (iFrame >= m_iColliderFrame2 && iFrame < m_iColliderFrame2 + 3)
		{
			if (pCollider->IsActive() == false)
			{
				pCollider->SetActive(true);
				pCollider->Set_AttackType((_uint)COLLIDER_ATTACK::MIDDLE);
				m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
		}


		if (iFrame >= m_iColliderFrame2 + 3)
		{
			if (pCollider->IsActive())
			{
				pCollider->SetActive(false);
				m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
		}
	}
	




}


void CState_WhiteZetsu_Attack_Kick_Server::Free()
{
	__super::Free();
}
