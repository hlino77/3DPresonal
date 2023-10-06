#include "stdafx.h"
#include "State_WhiteZetsu_HitMiddle.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_HitMiddle::CState_WhiteZetsu_HitMiddle(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_HitMiddle::CState_WhiteZetsu_HitMiddle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_HitMiddle::Initialize()
{
	m_iAnimIndex = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_UpFront_Head", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_HitMiddle::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);
}

void CState_WhiteZetsu_HitMiddle::Tick_State(_float fTimeDelta)
{

}

void CState_WhiteZetsu_HitMiddle::Exit_State()
{

}


void CState_WhiteZetsu_HitMiddle::Free()
{
	__super::Free();
}
