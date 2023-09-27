#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Naruto_Attack_ElbowStrike.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"


CState_Naruto_Attack_ElbowStrike::CState_Naruto_Attack_ElbowStrike(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Attack_ElbowStrike::CState_Naruto_Attack_ElbowStrike(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Attack_ElbowStrike::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_ElbowStrike", 2.0f);
	


	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iNextFrame = 51;
	m_iChangeFrame = 17;
	m_iInputNext = m_iNextFrame - 5;
	m_iStartFrame = 0;

	return S_OK;
}

void CState_Naruto_Attack_ElbowStrike::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, m_iStartFrame, m_iChangeFrame);
}

void CState_Naruto_Attack_ElbowStrike::Tick_State(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	if (pPlayerModel->Get_Anim_Frame(m_iAnimIndex) >= m_iInputNext)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			m_pPlayer->Set_State(L"Attack_JumpDoubleKick");
			return;
		}

	}

	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_Attack_ElbowStrike::Exit_State()
{
}

void CState_Naruto_Attack_ElbowStrike::Free()
{
	__super::Free();
}
