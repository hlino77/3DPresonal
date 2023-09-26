#include "stdafx.h"
#include "State_Sasuke_Idle.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Sasuke_Idle::CState_Sasuke_Idle(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Idle::CState_Sasuke_Idle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Idle::Initialize()
{
	m_iIdle_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pPlayer->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Sasuke_Idle::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_Sasuke_Idle::Tick_State(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
	{
		m_pPlayer->Set_State(L"Move");
	}
	if (KEY_TAP(KEY::LBTN))
		m_pPlayer->Set_State(L"Attack_Normal");
}

void CState_Sasuke_Idle::Exit_State()
{
}

void CState_Sasuke_Idle::Free()
{
	__super::Free();
}
