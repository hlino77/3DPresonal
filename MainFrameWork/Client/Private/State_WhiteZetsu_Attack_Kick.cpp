#include "stdafx.h"
#include "State_WhiteZetsu_Attack_Kick.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_Attack_Kick::CState_WhiteZetsu_Attack_Kick(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_Attack_Kick::CState_WhiteZetsu_Attack_Kick(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_Attack_Kick::Initialize()
{
	m_iAttack_JumpTurnKick = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Attack_JumpTurnKick", 1.2f);

	if (m_iAttack_JumpTurnKick == -1)
		return E_FAIL;

	return S_OK;
}

void CState_WhiteZetsu_Attack_Kick::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iAttack_JumpTurnKick, 0.3f, 0, 0);
}

void CState_WhiteZetsu_Attack_Kick::Tick_State(_float fTimeDelta)
{
	m_pMonster->Follow_ServerPos(0.01f, 4.0f * fTimeDelta);
}

void CState_WhiteZetsu_Attack_Kick::Exit_State()
{

}


void CState_WhiteZetsu_Attack_Kick::Free()
{
	__super::Free();
}
