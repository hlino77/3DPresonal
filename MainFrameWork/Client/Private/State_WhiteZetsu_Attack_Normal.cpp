#include "stdafx.h"
#include "State_WhiteZetsu_Attack_Normal.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Attack_Normal::CState_WhiteZetsu_Attack_Normal(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Attack_Normal::CState_WhiteZetsu_Attack_Normal(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Attack_Normal::Initialize()
{
	m_iAttack_DownStrike = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack_DownStrike", 1.0f);

	if (m_iAttack_DownStrike == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Attack_Normal::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAttack_DownStrike, 0.1f, 0, 0);
}

void CState_WhiteZetsu_Attack_Normal::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 0.1f);
}

void CState_WhiteZetsu_Attack_Normal::Exit_State()
{

}


void CState_WhiteZetsu_Attack_Normal::Free()
{
	__super::Free();
}
