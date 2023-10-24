#include "stdafx.h"
#include "State_Naruto_Land.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Naruto_Land::CState_Naruto_Land(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Land::CState_Naruto_Land(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Land::Initialize()
{
	m_iLand = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Land", 1.5f);

	if (m_iLand == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Land::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Land::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_Land::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iLand, 0.1f, 1, 0);
}

void CState_Naruto_Land::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Land::Exit_State()
{
}

void CState_Naruto_Land::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iLand))
		m_pPlayer->Set_State(L"Idle");


}

void CState_Naruto_Land::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Land::Free()
{
	__super::Free();
}
