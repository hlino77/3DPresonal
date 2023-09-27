#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Sasuke_RunLoop.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Sasuke_RunLoop::CState_Sasuke_RunLoop(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_RunLoop::CState_Sasuke_RunLoop(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_RunLoop::Initialize()
{
	m_iRun_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 1.0f);
	

	m_fAccel = 0.4f;
	m_fMaxSpeed = 5.0f;

	if (m_iRun_Loop == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Sasuke_RunLoop::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iRun_Loop, 0.2f, 0, 0);
}

void CState_Sasuke_RunLoop::Tick_State(_float fTimeDelta)
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

	_float fCurrSpeed = m_pPlayer->Get_MoveSpeed();
	if (vDir != Vec3(0.0f, 0.0f, 0.0f))
	{
		vDir.Normalize();

		if (fCurrSpeed < m_fMaxSpeed)
		{
			fCurrSpeed += m_fAccel;
			fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
			m_pPlayer->Set_MoveSpeed(fCurrSpeed);
		}
		m_pPlayer->Move_Dir(vDir, fCurrSpeed, fTimeDelta);
	}
	else
		m_pPlayer->Go_Straight(fCurrSpeed, fTimeDelta);


	if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))
	{
		m_pPlayer->Set_State(L"Run_End");
	}
}

void CState_Sasuke_RunLoop::Exit_State()
{
}

void CState_Sasuke_RunLoop::Free()
{
	__super::Free();
}
