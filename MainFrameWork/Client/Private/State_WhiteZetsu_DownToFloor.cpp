#include "stdafx.h"
#include "State_WhiteZetsu_DownToFloor.h"
#include "Monster_WhiteZetsu.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_DownToFloor::CState_WhiteZetsu_DownToFloor(const wstring& strStateName, CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_DownToFloor::CState_WhiteZetsu_DownToFloor(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_WhiteZetsu_DownToFloor::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_DownToFloor", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	
	return S_OK;
}

void CState_WhiteZetsu_DownToFloor::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
}

void CState_WhiteZetsu_DownToFloor::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_WhiteZetsu_DownToFloor::Exit_State()
{

}



void CState_WhiteZetsu_DownToFloor::Free()
{
	__super::Free();
}
