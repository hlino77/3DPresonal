#include "stdafx.h"
#include "State_Naruto_Attack_Punch_Right.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Client_Defines.h"

CState_Naruto_Attack_Punch_Right::CState_Naruto_Attack_Punch_Right(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Attack_Punch_Right::CState_Naruto_Attack_Punch_Right(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Attack_Punch_Right::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_Punch_Right", 2.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iNextFrame = 17;
	m_iChangeFrame = 16;
	m_iInputNext = m_iNextFrame - 5;
	m_iStartFrame = 0;

	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Attack_Punch_Right::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Attack_Punch_Right::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_Attack_Punch_Right::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, m_iStartFrame, m_iChangeFrame);
}

void CState_Naruto_Attack_Punch_Right::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Attack_Punch_Right::Exit_State()
{
}

void CState_Naruto_Attack_Punch_Right::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	if (pPlayerModel->Get_Anim_Frame(m_iAnimIndex) >= m_iInputNext)
	{
		if (KEY_TAP(KEY::LBTN))
		{
			m_pPlayer->Set_State(L"Attack_Normal_cmb03");
			return;
		}

	}

	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_Attack_Punch_Right::Tick_State_NoneControl(_float fTimeDelta)
{
}

void CState_Naruto_Attack_Punch_Right::Free()
{
	__super::Free();
}
