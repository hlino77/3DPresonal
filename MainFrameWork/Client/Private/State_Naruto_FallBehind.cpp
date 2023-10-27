#include "stdafx.h"
#include "State_Naruto_FallBehind.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Naruto_FallBehind::CState_Naruto_FallBehind(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_FallBehind::CState_Naruto_FallBehind(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_FallBehind::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Fall_Behind_Loop", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_FallBehind::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_FallBehind::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_FallBehind::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Naruto_FallBehind::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_FallBehind::Exit_State()
{

}

void CState_Naruto_FallBehind::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_RigidBody()->Is_Gruond())
		m_pPlayer->Set_State(L"DownToFloor");
}

void CState_Naruto_FallBehind::Tick_State_NoneControl(_float fTimeDelta)
{

}



void CState_Naruto_FallBehind::Free()
{
	__super::Free();
}
