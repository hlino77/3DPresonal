#include "stdafx.h"
#include "State_WhiteZetsu_Attack_Normal_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"
#include "ColliderSphere.h"

CState_WhiteZetsu_Attack_Normal_Server::CState_WhiteZetsu_Attack_Normal_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Attack_Normal_Server::CState_WhiteZetsu_Attack_Normal_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Attack_Normal_Server::Initialize()
{
	m_iAttack_DownStrike = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack_DownStrike", 1.0f);

	if (m_iAttack_DownStrike == -1)
		return E_FAIL;


	m_iColliderFrame = 15;

	return S_OK;
}

void CState_WhiteZetsu_Attack_Normal_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAttack_DownStrike, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Attack_Normal_Server::Tick_State(_float fTimeDelta)
{
	Update_Collider();

	if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAttack_DownStrike))
		m_pMonster->Set_State(L"Idle");
}

void CState_WhiteZetsu_Attack_Normal_Server::Exit_State()
{
	CSphereCollider* pCollider = m_pMonster->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	pCollider->SetActive(false);
	m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
}

void CState_WhiteZetsu_Attack_Normal_Server::Update_Collider()
{
	_uint iFrame = m_pMonster->Get_ModelCom()->Get_Anim_Frame(m_iAttack_DownStrike);
	CSphereCollider* pCollider = m_pMonster->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	if (iFrame >= m_iColliderFrame && iFrame < m_iColliderFrame + 3)
	{
		if (pCollider->IsActive() == false)
		{
			pCollider->SetActive(true);
			pCollider->Set_AttackType((_uint)COLLIDER_ATTACK::MIDDLE);
			m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		}
	}


	if (iFrame >= m_iColliderFrame + 3)
	{
		if (pCollider->IsActive())
		{
			pCollider->SetActive(false);
			m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
		}
	}
}


void CState_WhiteZetsu_Attack_Normal_Server::Free()
{
	__super::Free();
}
