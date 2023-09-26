#include "stdafx.h"
#include "State_Sasuke_Move.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Sasuke_Move::CState_Sasuke_Move(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Move::CState_Sasuke_Move(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Move::Initialize()
{
	m_iRun_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 1.0f);
	m_iRun_End = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Run_End", 1.0f);


	m_fAccel = 0.4f;
	m_fMaxSpeed = 5.0f;

	if (m_iRun_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Sasuke_Move::Enter_State()
{
	m_eState = RUNSTART;
	m_pPlayer->Reserve_Animation(m_iRun_Loop, 0.2f, 0, 0);
	m_bStop = false;
	m_fCurrSpeed = 0.0f;
}

void CState_Sasuke_Move::Tick_State(_float fTimeDelta)
{
	switch (m_eState)
	{
	case RUNSTART:
		UpdateMove(fTimeDelta);
		break;
	case RUNEND:
		UpdateEnd(fTimeDelta);
		break;
	}
}

void CState_Sasuke_Move::Exit_State()
{
	m_eState = RUNEND;
	m_bStop = false;
}

void CState_Sasuke_Move::UpdateMove(_float fTimeDelta)
{
	Vec3 vDir(0.0f, 0.0f, 0.0f);

	
	if (KEY_HOLD(KEY::W))
	{
		vDir += m_pPlayer->Make_StraightDir();
	}
	if (KEY_HOLD(KEY::S))
	{
		vDir += m_pPlayer->Make_BackDir();
	}
	if (KEY_HOLD(KEY::D))
	{
		vDir += m_pPlayer->Make_RightDir();
	}
	if (KEY_HOLD(KEY::A))
	{
		vDir += m_pPlayer->Make_LeftDir();
	}
	
	if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))
	{
		if (m_pPlayer->Get_ModelCom()->Get_CurrAnim() == m_iRun_Loop || m_pPlayer->Get_ModelCom()->Get_CurrAnim() == m_iRun_End)
			m_eState = RUNEND;
		else
			m_pPlayer->Set_State(L"Idle");
	}
	

	if (vDir != Vec3(0.0f, 0.0f, 0.0f))
	{
		vDir.Normalize();
		if (m_fCurrSpeed < m_fMaxSpeed)
		{
			m_fCurrSpeed += m_fAccel;
			m_fCurrSpeed = min(m_fCurrSpeed, m_fMaxSpeed);
		}
		m_pPlayer->Move_Dir(vDir, m_fCurrSpeed, fTimeDelta);
	}
	else
		m_pPlayer->Go_Straight(m_fCurrSpeed, fTimeDelta);
}

void CState_Sasuke_Move::UpdateEnd(_float fTimeDelta)
{
	if (m_bStop)
	{
		if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D))
		{
			m_pPlayer->Reserve_Animation(m_iRun_Loop, 0.2f, 0, 0);
			m_eState = RUNSTART;
			m_bStop = false;
		}
		else
		{
			if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iRun_End))
			{
				m_pPlayer->Set_State(L"Idle");
				return;
			}
			else
				m_fCurrSpeed *= 0.93f;
		}	
	}
	else
	{
		m_bStop = true;
		m_pPlayer->Reserve_Animation(m_iRun_End, 0.1f, 0, 0);
	}
	
	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D))
	{
		m_pPlayer->Reserve_Animation(m_iRun_Loop, 0.2f, 0, 0);
		m_eState = RUNSTART;
		m_bStop = false;
	}
	m_pPlayer->Go_Straight(m_fCurrSpeed, fTimeDelta);
}

void CState_Sasuke_Move::Free()
{
	__super::Free();
}
