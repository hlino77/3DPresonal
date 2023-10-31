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
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack_DownStrike", 1.2f);

	if (m_iAnimIndex == -1)
		return E_FAIL;


	m_iColliderFrame = 18;

	m_fCollierTime = 0.1f;

	return S_OK;
}

void CState_WhiteZetsu_Attack_Normal_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Attack_Normal_Server::Tick_State(_float fTimeDelta)
{
	Update_Collider(fTimeDelta);

	if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pMonster->Set_State(L"Idle");
}

void CState_WhiteZetsu_Attack_Normal_Server::Exit_State()
{
	CSphereCollider* pCollider = m_pMonster->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);
	pCollider->SetActive(false);
	pCollider->Reset_Attack();
	m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
}

void CState_WhiteZetsu_Attack_Normal_Server::Update_Collider(_float fTimeDelta)
{
	CModel* pMonsterModel = m_pMonster->Get_ModelCom();

	CSphereCollider* pCollider = m_pMonster->Get_Colider((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	_uint iFrame = pMonsterModel->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame < m_iColliderFrame)
	{
		m_bAttack = false;
		return;
	}


	if (m_bAttack == false)
	{
		if (iFrame >= m_iColliderFrame)
		{
			if (pCollider->IsActive() == false)
			{
				pCollider->SetActive(true);
				pCollider->Set_AttackCollider(1, (_uint)COLLIDER_ATTACK::MIDDLE, true);
				m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

				m_fCurrTime = 0.0f;
				m_bAttack = true;
			}
		}
	}
	else
	{
		if (pCollider->IsActive() == true)
		{
			m_fCurrTime += fTimeDelta;

			if (m_fCurrTime >= m_fCollierTime)
			{
				pCollider->SetActive(false);
				pCollider->Reset_Attack();
				m_pMonster->Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);
			}
		}
	}

}


void CState_WhiteZetsu_Attack_Normal_Server::Free()
{
	__super::Free();
}
