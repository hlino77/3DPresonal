#include "stdafx.h"
#include "State_Sasuke_FallBehind.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Sasuke_FallBehind::CState_Sasuke_FallBehind(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_FallBehind::CState_Sasuke_FallBehind(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_FallBehind::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Fall_Behind_Loop", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_FallBehind::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_FallBehind::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_FallBehind::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Sasuke_FallBehind::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_FallBehind::Exit_State()
{

}

void CState_Sasuke_FallBehind::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_RigidBody()->Is_Gruond())
		m_pPlayer->Set_State(L"DownToFloor");
}

void CState_Sasuke_FallBehind::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}



void CState_Sasuke_FallBehind::Free()
{
	__super::Free();
}
