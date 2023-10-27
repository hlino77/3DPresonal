#include "stdafx.h"
#include "State_WhiteZetsu_Idle.h"
#include "Monster_WhiteZetsu.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_WhiteZetsu_Idle::CState_WhiteZetsu_Idle(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Idle::CState_WhiteZetsu_Idle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Idle::Initialize()
{
	m_iIdle_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pMonster->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Idle::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Idle::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_WhiteZetsu_Idle::Exit_State()
{

}


void CState_WhiteZetsu_Idle::Free()
{
	__super::Free();
}
