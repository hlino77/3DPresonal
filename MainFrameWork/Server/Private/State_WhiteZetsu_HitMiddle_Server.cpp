#include "stdafx.h"
#include "State_WhiteZetsu_HitMiddle_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_HitMiddle_Server::CState_WhiteZetsu_HitMiddle_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitMiddle_Server::CState_WhiteZetsu_HitMiddle_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_HitMiddle_Server::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_UpFront_Head", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitMiddle_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
}

void CState_WhiteZetsu_HitMiddle_Server::Tick_State(_float fTimeDelta)
{
	CModel* pModel = m_pMonster->Get_ModelCom();
	if (pModel == nullptr || pModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	if (pModel->Is_AnimationEnd(m_iAnimIndex))
		m_pMonster->Set_State(L"Idle");
}

void CState_WhiteZetsu_HitMiddle_Server::Exit_State()
{

}


void CState_WhiteZetsu_HitMiddle_Server::Free()
{
	__super::Free();
}
