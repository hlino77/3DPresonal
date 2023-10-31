#include "stdafx.h"
#include "State_Naruto_DownToFloor.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Naruto_DownToFloor::CState_Naruto_DownToFloor(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_DownToFloor::CState_Naruto_DownToFloor(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_DownToFloor::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_DownToFloor", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_DownToFloor::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_DownToFloor::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_DownToFloor::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
}

void CState_Naruto_DownToFloor::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_DownToFloor::Exit_State()
{

}

void CState_Naruto_DownToFloor::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"GetUp");
}

void CState_Naruto_DownToFloor::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}



void CState_Naruto_DownToFloor::Free()
{
	__super::Free();
}
