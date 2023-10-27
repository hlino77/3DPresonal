#include "stdafx.h"
#include "State_Deidara_Idle.h"
#include "Boss_Deidara.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_Deidara_Idle::CState_Deidara_Idle(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Idle::CState_Deidara_Idle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Idle::Initialize()
{
	m_iIdle_Loop = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_Idle::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_Deidara_Idle::Tick_State(_float fTimeDelta)
{
	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_Idle::Exit_State()
{

}


void CState_Deidara_Idle::Free()
{
	__super::Free();
}
