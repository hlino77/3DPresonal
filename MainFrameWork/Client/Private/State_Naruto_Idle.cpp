#include "stdafx.h"
#include "State_Naruto_Idle.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Pool.h"
#include "Explosion.h"
#include "ExplosionRing.h"
#include "Smoke_24.h"
#include "LineCircle.h"


CState_Naruto_Idle::CState_Naruto_Idle(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Idle::CState_Naruto_Idle(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Idle::Initialize()
{
	m_iIdle_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pPlayer->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Idle::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Idle::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_Idle::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_Naruto_Idle::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Idle::Exit_State()
{

}

void CState_Naruto_Idle::Tick_State_Control(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
	{
		m_pPlayer->Set_State(L"Walk_Loop");
		return;
	}

	if (KEY_TAP(KEY::LBTN))
	{
		m_pPlayer->Set_State(L"Attack_Normal_cmb01");
		return;
	}
	

	if (KEY_TAP(KEY::SPACE))
	{
		m_pPlayer->Set_State(L"Jump");
		return;
	}


	if (KEY_TAP(KEY::P))
	{
		m_pPlayer->Effect_Hit();
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



	if (KEY_TAP(KEY::O))
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);


		Safe_Release(pGameInstance);
	}

}

void CState_Naruto_Idle::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Idle::Free()
{
	__super::Free();
}
