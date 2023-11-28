#include "stdafx.h"
#include "State_WhiteZetsu_Die.h"
#include "Monster_WhiteZetsu.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"


CState_WhiteZetsu_Die::CState_WhiteZetsu_Die(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Die::CState_WhiteZetsu_Die(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Die::Initialize()
{

	return S_OK;
}

void CState_WhiteZetsu_Die::Enter_State()
{
	m_pMonster->Set_Die();
	m_pMonster->Effect_Die();
}

void CState_WhiteZetsu_Die::Tick_State(_float fTimeDelta)
{
	m_pMonster->Set_Active(false);
}

void CState_WhiteZetsu_Die::Exit_State()
{

}


void CState_WhiteZetsu_Die::Free()
{
	__super::Free();
}
