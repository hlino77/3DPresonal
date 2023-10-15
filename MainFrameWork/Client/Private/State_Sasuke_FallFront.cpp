#include "stdafx.h"
#include "State_Sasuke_FallFront.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Sasuke_FallFront::CState_Sasuke_FallFront(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_FallFront::CState_Sasuke_FallFront(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_FallFront::Initialize()
{
	m_iFall_Front = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Fall_Front_Loop", 1.0f);

	if (m_iFall_Front == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_FallFront::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_FallFront::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_FallFront::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iFall_Front, 0.2f, 0, 0);
	m_pPlayer->Get_RigidBody()->UseGravity(true);
	m_pPlayer->Set_Render(true);
}

void CState_Sasuke_FallFront::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_FallFront::Exit_State()
{

}

void CState_Sasuke_FallFront::Tick_State_Control(_float fTimeDelta)
{
	if (KEY_TAP(KEY::SPACE))
	{
		m_pPlayer->Set_State(L"DoubleJump");
		return;
	}

	if (m_pPlayer->Get_RigidBody()->UseGravity() == false)
	{
		m_pPlayer->Set_State(L"Idle");
		return;
	}
}

void CState_Sasuke_FallFront::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 0.1f);
}

void CState_Sasuke_FallFront::Free()
{
	__super::Free();
}
