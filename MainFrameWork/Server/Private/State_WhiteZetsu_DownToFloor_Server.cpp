#include "stdafx.h"
#include "State_WhiteZetsu_DownToFloor_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_DownToFloor_Server::CState_WhiteZetsu_DownToFloor_Server(const wstring& strStateName, CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_DownToFloor_Server::CState_WhiteZetsu_DownToFloor_Server(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_WhiteZetsu_DownToFloor_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_DownToFloor", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	
	return S_OK;
}

void CState_WhiteZetsu_DownToFloor_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
}

void CState_WhiteZetsu_DownToFloor_Server::Tick_State(_float fTimeDelta)
{
	if (m_pMonster->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
	{
		if (m_pMonster->Get_Hp() <= 0)
		{
			m_pMonster->Set_State(L"Die");
		}
		else
			m_pMonster->Set_State(L"GetUp");
	}
		
}

void CState_WhiteZetsu_DownToFloor_Server::Exit_State()
{

}



void CState_WhiteZetsu_DownToFloor_Server::Free()
{
	__super::Free();
}
