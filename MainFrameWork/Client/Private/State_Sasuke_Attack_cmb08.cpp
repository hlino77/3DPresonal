#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Sasuke_Attack_cmb08.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"


CState_Sasuke_Attack_cmb08::CState_Sasuke_Attack_cmb08(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Attack_cmb08::CState_Sasuke_Attack_cmb08(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Attack_cmb08::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb08", 1.0f);
	


	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iNextFrame = 14;
	m_iChangeFrame = 14;
	m_iInputNext = m_iNextFrame - 5;
	m_iStartFrame = 1;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Attack_cmb08::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Attack_cmb08::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_Attack_cmb08::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, m_iStartFrame, m_iChangeFrame);
}

void CState_Sasuke_Attack_cmb08::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Attack_cmb08::Exit_State()
{
}

void CState_Sasuke_Attack_cmb08::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	if (pPlayerModel->Get_Anim_Frame(m_iAnimIndex) >= m_iInputNext)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			m_pPlayer->Set_State(L"Attack_Normal_cmb04");
			return;
		}

	}

	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Sasuke_Attack_cmb08::Tick_State_NoneControl(_float fTimeDelta)
{
}

void CState_Sasuke_Attack_cmb08::Free()
{
	__super::Free();
}
