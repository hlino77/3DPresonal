#include "stdafx.h"
#include "State_Madara_DownToFloor_Server.h"
#include "Boss_Madara_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Madara_DownToFloor_Server::CState_Madara_DownToFloor_Server(const wstring& strStateName, CBoss_Madara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_DownToFloor_Server::CState_Madara_DownToFloor_Server(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Madara_DownToFloor_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_DownToFloor", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	
	return S_OK;
}

void CState_Madara_DownToFloor_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
}

void CState_Madara_DownToFloor_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
	{
		if (m_pBoss->Get_Hp() <= 0)
		{
			m_pBoss->Set_State(L"Die");
		}
		else
			m_pBoss->Set_State(L"GetUp");
	}
		
}

void CState_Madara_DownToFloor_Server::Exit_State()
{

}



void CState_Madara_DownToFloor_Server::Free()
{
	__super::Free();
}
