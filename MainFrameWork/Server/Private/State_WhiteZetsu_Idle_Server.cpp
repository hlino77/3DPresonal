#include "stdafx.h"
#include "State_WhiteZetsu_Idle_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Idle_Server::CState_WhiteZetsu_Idle_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Idle_Server::CState_WhiteZetsu_Idle_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Idle_Server::Initialize()
{
	m_iIdle_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);

	if (m_iIdle_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Idle_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Idle_Server::Tick_State(_float fTimeDelta)
{
	m_pMonster->Update_NearTarget(fTimeDelta);

	_float fDistance = m_pMonster->Get_NearTargetDistance();

	if (fDistance <= 1.0f)
	{
		m_fAttackDelay += fTimeDelta;

		if (m_fAttackDelay >= 1.0f)
		{
			m_pMonster->Attack_Random();
			m_fAttackDelay = 0.0f;
		}
	}


	Vec3 vTargetPos = m_pMonster->Get_TargetPos();

}

void CState_WhiteZetsu_Idle_Server::Exit_State()
{

}


void CState_WhiteZetsu_Idle_Server::Free()
{
	__super::Free();
}
