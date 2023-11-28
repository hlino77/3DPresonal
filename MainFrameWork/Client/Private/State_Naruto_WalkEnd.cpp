#include "stdafx.h"
#include "State_Naruto_WalkEnd.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Naruto_WalkEnd::CState_Naruto_WalkEnd(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	: CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_WalkEnd::CState_Naruto_WalkEnd(const CState& rhs, CStateMachine* pMachine)
	: CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_WalkEnd::Initialize()
{
	m_iWalk_End = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Walk_End", 1.0f);

	if (m_iWalk_End == -1)
		return E_FAIL;


	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_WalkEnd::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_WalkEnd::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_WalkEnd::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iWalk_End, 0.1f, 5, 0);

	m_pPlayer->Set_MoveSpeed(0.0f);


	if (m_pPlayer->Is_Wall())
		m_pPlayer->DisAppear_FootTrail();
}

void CState_Naruto_WalkEnd::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_WalkEnd::Exit_State()
{

}

void CState_Naruto_WalkEnd::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iWalk_End))
	{
		m_pPlayer->Set_State(L"Idle");
		return;
	}

	if (KEY_TAP(KEY::NUM_1))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENGAN))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENGAN, false);
			m_pPlayer->Set_State(L"Rasengun_Start");
			return;
		}
	}

	if (KEY_TAP(KEY::NUM_2))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENSYURIKEN))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENSYURIKEN, false);
			m_pPlayer->Set_State(L"RasenSyuriken");
			return;
		}
	}


	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D))
	{
		m_pPlayer->Set_State(L"Walk_Loop");
		return;
	}



	if (KEY_TAP(KEY::LBTN))
	{
		m_pPlayer->Set_State(L"Attack_Normal_cmb01");
		return;
	}
}

void CState_Naruto_WalkEnd::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}



void CState_Naruto_WalkEnd::Free()
{
	__super::Free();
}
