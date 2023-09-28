#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Naruto_Attack_JumpDoubleKick.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Naruto_Attack_JumpDoubleKick::CState_Naruto_Attack_JumpDoubleKick(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Attack_JumpDoubleKick::CState_Naruto_Attack_JumpDoubleKick(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Attack_JumpDoubleKick::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Attack_JumpDoubleKick", 2.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	m_iNextFrame = 59;
	m_iChangeFrame = 51;
	m_iInputNext = m_iNextFrame - 5;
	m_iStartFrame = 0;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Attack_JumpDoubleKick::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Attack_JumpDoubleKick::Tick_State_NoneControl;



	return S_OK;
}

void CState_Naruto_Attack_JumpDoubleKick::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, m_iStartFrame, m_iChangeFrame);
}

void CState_Naruto_Attack_JumpDoubleKick::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Attack_JumpDoubleKick::Exit_State()
{
}

void CState_Naruto_Attack_JumpDoubleKick::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	if (pPlayerModel->Get_CurrAnim() != m_iAnimIndex)
		return;

	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Naruto_Attack_JumpDoubleKick::Tick_State_NoneControl(_float fTimeDelta)
{
}

void CState_Naruto_Attack_JumpDoubleKick::Free()
{
	__super::Free();
}
