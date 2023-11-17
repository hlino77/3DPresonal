#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Naruto_WalkLoop.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Naruto_WalkLoop::CState_Naruto_WalkLoop(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_WalkLoop::CState_Naruto_WalkLoop(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_WalkLoop::Initialize()
{
	m_iWalk_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Walk_Loop", 1.0f);
	

	m_fAccel = 10.0f;
	m_fMaxSpeed = 2.0f;

	if (m_iWalk_Loop == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_WalkLoop::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_WalkLoop::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_WalkLoop::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iWalk_Loop, 0.2f, 0, 0);

	if (m_pPlayer->Is_Control())
	{
		Vec3 vDir = Make_MoveDir();
		vDir.Normalize();
		Set_TargetPos(vDir);
	}

	m_pPlayer->Set_MoveSpeed(0.0f);

	if (m_pPlayer->Is_Wall())
		m_pPlayer->Appear_FootTrail();
}

void CState_Naruto_WalkLoop::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_WalkLoop::Exit_State()
{

}

void CState_Naruto_WalkLoop::Tick_State_Control(_float fTimeDelta)
{
	Vec3 vDir = Make_MoveDir();

	_float fCurrSpeed = m_pPlayer->Get_MoveSpeed();
	if (vDir != Vec3(0.0f, 0.0f, 0.0f))
	{
		vDir.Normalize();
		Set_TargetPos(vDir);

		if (fCurrSpeed < m_fMaxSpeed)
		{
			fCurrSpeed += m_fAccel * fTimeDelta;
			fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
			m_pPlayer->Set_MoveSpeed(fCurrSpeed);
		}
		m_pPlayer->Move_Dir(vDir, fCurrSpeed, fTimeDelta);
	}
	else
		m_pPlayer->Go_Straight(fCurrSpeed, fTimeDelta);



	if (fCurrSpeed >= m_fMaxSpeed)
	{
		m_pPlayer->Set_State(L"Run_Loop");
		return;
	}

	if (KEY_TAP(KEY::NUM_1))
	{
		m_pPlayer->Set_State(L"Rasengun_Start");
		return;
	}


	if (KEY_TAP(KEY::SPACE))
	{
		m_pPlayer->Set_State(L"Jump");
		return;
	}
		

	if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))
	{
		m_pPlayer->Set_State(L"Walk_End");
		return;
	}

	if (KEY_TAP(KEY::LBTN))
	{
		m_pPlayer->Set_State(L"Attack_Normal_cmb01");
		return;
	}
	
}

void CState_Naruto_WalkLoop::Tick_State_NoneControl(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();
	Vec3 vCurrPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
	Vec3 vServerPos(m_pPlayer->Get_TargetMatrix().m[3]);

	if ((vServerPos - vCurrPos).Length() > 0.2f)
	{
		vCurrPos = Vec3::Lerp(vCurrPos, vServerPos, 0.01f);
	}

	Vec3 vDir = vTargetPos - vCurrPos;
	vDir.Normalize();

	_float fCurrSpeed = m_pPlayer->Get_MoveSpeed();

	if (fCurrSpeed < m_fMaxSpeed)
	{
		fCurrSpeed += m_fAccel * fTimeDelta;
		fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
		m_pPlayer->Set_MoveSpeed(fCurrSpeed);
	}

	m_pPlayer->Move_Dir(vDir, fCurrSpeed, fTimeDelta);

	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_WalkLoop::Set_TargetPos(Vec3 vDir)
{
	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

	Vec3 vTargetPos = vPos + vDir * 5.0f;
	m_pPlayer->Set_TargetPos(vTargetPos);
}


Vec3 CState_Naruto_WalkLoop::Make_MoveDir()
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

	return vDir;
}

void CState_Naruto_WalkLoop::Free()
{
	__super::Free();
}
