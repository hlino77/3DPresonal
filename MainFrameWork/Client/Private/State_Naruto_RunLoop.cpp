#include "stdafx.h"
#include "Client_Defines.h"
#include "State_Naruto_RunLoop.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Naruto_RunLoop::CState_Naruto_RunLoop(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_RunLoop::CState_Naruto_RunLoop(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_RunLoop::Initialize()
{
	m_iRun_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 1.0f);
	

	m_fAccel = 0.4f;
	m_fMaxSpeed = 5.0f;

	if (m_iRun_Loop == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_RunLoop::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_RunLoop::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_RunLoop::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iRun_Loop, 0.2f, 0, 0);

	if (m_pPlayer->Is_Control())
	{
		Vec3 vDir = Make_MoveDir();
		vDir.Normalize();
		Set_TargetPos(vDir);
	}
	else
		Set_BezierPos();
}

void CState_Naruto_RunLoop::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_RunLoop::Exit_State()
{

}

void CState_Naruto_RunLoop::Tick_State_Control(_float fTimeDelta)
{
	Vec3 vDir = Make_MoveDir();


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


	Set_TargetPos(vDir);

	if (KEY_NONE(KEY::W) && KEY_NONE(KEY::A) && KEY_NONE(KEY::S) && KEY_NONE(KEY::D))
	{
		m_pPlayer->Set_State(L"Run_End");
	}
}

void CState_Naruto_RunLoop::Tick_State_NoneControl(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	_float fCurrSpeed = m_pPlayer->Get_MoveSpeed();

	if (fCurrSpeed < m_fMaxSpeed)
	{
		fCurrSpeed += m_fAccel;
		fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
		m_pPlayer->Set_MoveSpeed(fCurrSpeed);
	}

	if (m_vLastPos != m_pPlayer->Get_TargetPos())
		Set_BezierPos();
	

	m_fBezierRatio += fCurrSpeed * fTimeDelta;
	
	Vec3 vBezier1, vBezier2, vResultPos;

	vBezier1 = Vec3::Lerp(m_vStartPos, m_vMiddlePos, m_fBezierRatio);
	vBezier2 = Vec3::Lerp(m_vMiddlePos, m_vLastPos, m_fBezierRatio);
	vResultPos = Vec3::Lerp(vBezier1, vBezier2, m_fBezierRatio);

	Vec3 vCurrPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);

	Vec3 vDir = vResultPos - vCurrPos;
	vDir.Normalize();

	m_pPlayer->Move_Dir(vDir, fCurrSpeed ,fTimeDelta);
}

Vec3 CState_Naruto_RunLoop::Make_MoveDir()
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

void CState_Naruto_RunLoop::Set_TargetPos(Vec3 vDir)
{
	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

	Vec3 vTargetPos = vPos + vDir * 1.5f;
	m_pPlayer->Set_TargetPos(vTargetPos);
}

void CState_Naruto_RunLoop::Set_BezierPos()
{
	m_vStartPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
	m_vMiddlePos = Vec3(m_pPlayer->Get_TargetMatrix().m[3]);
	m_vLastPos = m_pPlayer->Get_TargetPos();
	m_fBezierRatio = 0.0f;
}

void CState_Naruto_RunLoop::Free()
{
	__super::Free();
}
