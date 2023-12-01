#include "stdafx.h"
#include "State_Sasuke_Land.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Sasuke_Land::CState_Sasuke_Land(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Land::CState_Sasuke_Land(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Land::Initialize()
{
	m_iLand = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Land", 1.5f);

	if (m_iLand == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Land::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Land::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_Land::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iLand, 0.1f, 1, 0);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"JumpGround.wav", g_fVolume * 0.4f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
}

void CState_Sasuke_Land::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Land::Exit_State()
{
}

void CState_Sasuke_Land::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iLand))
		m_pPlayer->Set_State(L"Idle");
}

void CState_Sasuke_Land::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Land::Free()
{
	__super::Free();
}
