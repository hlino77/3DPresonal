#include "stdafx.h"
#include "State_Naruto_Attack_Normal.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Naruto_Attack_Normal::CState_Naruto_Attack_Normal(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Attack_Normal::CState_Naruto_Attack_Normal(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Attack_Normal::Initialize()
{
	m_iAttack_Punch_Left.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_Punch_Left", 2.0f);
	m_iAttack_Punch_Right.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_Punch_Right", 2.0f);
	m_iAttack_ElbowStrike.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_ElbowStrike", 2.0f);
	m_iAttack_JumpDoubleKick.iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_JumpDoubleKick", 2.0f);
	


	if (m_iAttack_Punch_Left.iAnimIndex == -1 ||
		m_iAttack_Punch_Right.iAnimIndex == -1 ||
		m_iAttack_ElbowStrike.iAnimIndex == -1 ||
		m_iAttack_JumpDoubleKick.iAnimIndex == -1)
		return E_FAIL;

	m_iAttack_Punch_Left.iChangeFrame = 16;
	m_iAttack_Punch_Right.iChangeFrame = 17;
	m_iAttack_ElbowStrike.iChangeFrame = 51;
	m_iAttack_JumpDoubleKick.iChangeFrame = 59;


	m_iAttack_Punch_Left.iInputNext = m_iAttack_Punch_Left.iChangeFrame - 5;
	m_iAttack_Punch_Right.iInputNext = m_iAttack_Punch_Right.iChangeFrame - 5;
	m_iAttack_ElbowStrike.iInputNext = m_iAttack_ElbowStrike.iChangeFrame - 5;
	m_iAttack_JumpDoubleKick.iInputNext = 2;


	m_iAttack_Punch_Left.iNextFrame = 0;
	m_iAttack_Punch_Right.iNextFrame = 0;
	m_iAttack_ElbowStrike.iNextFrame = 0;
	m_iAttack_JumpDoubleKick.iNextFrame = 0;


	m_Attack.push_back(m_iAttack_Punch_Left);
	m_Attack.push_back(m_iAttack_Punch_Right);
	m_Attack.push_back(m_iAttack_ElbowStrike);
	m_Attack.push_back(m_iAttack_JumpDoubleKick);

	return S_OK;
}

void CState_Naruto_Attack_Normal::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAttack_Punch_Left.iAnimIndex, 0.1f, 0, 0);
	m_iCurrAttack = 0;
}

void CState_Naruto_Attack_Normal::Tick_State(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_Attack[m_iCurrAttack].iAnimIndex)
		return;

	if (pPlayerModel->Get_Anim_Frame(m_Attack[m_iCurrAttack].iAnimIndex) >= m_Attack[m_iCurrAttack].iInputNext)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			if (m_iCurrAttack < m_Attack.size() - 1)
			{
				m_pPlayer->Reserve_Animation(m_Attack[m_iCurrAttack + 1].iAnimIndex, 0.1f, m_Attack[m_iCurrAttack].iNextFrame, m_Attack[m_iCurrAttack].iChangeFrame);
				++m_iCurrAttack;
			}
		}

	}

	if (pPlayerModel->Is_AnimationEnd(m_Attack[m_iCurrAttack].iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_Attack_Normal::Exit_State()
{
}

void CState_Naruto_Attack_Normal::Free()
{
	__super::Free();
}
