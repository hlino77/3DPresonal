#include "stdafx.h"
#include "State_Deidara_FallBehind_Server.h"
#include "Boss_Deidara_Server.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Deidara_FallBehind_Server::CState_Deidara_FallBehind_Server(const wstring& strStateName, CBoss_Deidara_Server* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_FallBehind_Server::CState_Deidara_FallBehind_Server(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Deidara_FallBehind_Server::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Fall_Behind_Loop", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_FallBehind_Server::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Deidara_FallBehind_Server::Tick_State(_float fTimeDelta)
{
	if (m_pBoss->Get_RigidBody()->Is_Gruond())
		m_pBoss->Set_State(L"DownToFloor");
}

void CState_Deidara_FallBehind_Server::Exit_State()
{

}

void CState_Deidara_FallBehind_Server::Free()
{
	__super::Free();
}
