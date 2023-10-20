#include "stdafx.h"
#include "State_WhiteZetsu_Attack_Punch.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Attack_Punch::CState_WhiteZetsu_Attack_Punch(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Attack_Punch::CState_WhiteZetsu_Attack_Punch(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Attack_Punch::Initialize()
{
	m_iAttack_Punch = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack_Punch", 1.0f);

	if (m_iAttack_Punch == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Attack_Punch::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAttack_Punch, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Attack_Punch::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 0.1f);
}

void CState_WhiteZetsu_Attack_Punch::Exit_State()
{

}


void CState_WhiteZetsu_Attack_Punch::Free()
{
	__super::Free();
}
