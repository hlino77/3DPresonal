#include "stdafx.h"
#include "State_Sasuke_Idle.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "PickingMgr.h"
#include "Pool.h"
#include "Lightning_A_01.h"
#include "Lightning_A_02.h"
#include "Lightning_B_01.h"
#include "Lightning_B_02.h"
#include "Lightning_B_03.h"
#include "Lightning_C_01.h"
#include "Lightning_Chidori.h"


CState_Sasuke_Idle::CState_Sasuke_Idle(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Idle::CState_Sasuke_Idle(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Idle::Initialize()
{
	m_iIdle_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pPlayer->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Idle::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Idle::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_Idle::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_Sasuke_Idle::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Idle::Exit_State()
{
}

void CState_Sasuke_Idle::Tick_State_Control(_float fTimeDelta)
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



	if (KEY_TAP(KEY::NUM_1))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Sasuke::SASUKE_SKILL::CHIDORI))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Sasuke::SASUKE_SKILL::CHIDORI, false);
			m_pPlayer->Set_State(L"Chidori_Start");
			return;
		}
	}

	if (KEY_TAP(KEY::NUM_2))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Sasuke::SASUKE_SKILL::FIREBALL))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Sasuke::SASUKE_SKILL::FIREBALL, false);
			m_pPlayer->Set_State(L"FireBall");
			return;
		}
	}



	if (KEY_TAP(KEY::O))
	{
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vPos.y += 1.0f;

		CLightning_Chidori* pLightning = CPool<CLightning_Chidori>::Get_Obj();

		pLightning->Appear(vPos);
	}




}

void CState_Sasuke_Idle::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Idle::Free()
{
	__super::Free();
}
