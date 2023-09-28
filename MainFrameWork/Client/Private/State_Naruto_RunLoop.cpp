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
	{
		Push_Spline();
		Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
		vLook.Normalize();
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);

		m_vPrevPos = vPos - vLook;
		m_Spline.front().vPos += vLook * 0.5f;
	}
		
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
	
	Vec3 vServerPos(m_pPlayer->Get_TargetMatrix().m[3]);

	if (m_Spline.back().vPos != vServerPos)
		Push_Spline();
	
	m_fSplineRatio += fCurrSpeed * fTimeDelta;

	if (m_fSplineRatio >= 1.0f)
	{
		m_fSplineRatio -= 1.0f;
		m_Spline.pop_front();
	}

	Vec3 vCurrPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);

	Vec3 vResult =  XMVectorCatmullRom(m_vPrevPos, vCurrPos, m_Spline.front().vPos, m_Spline.front().vTargetPos, m_fSplineRatio);
	m_vPrevPos = vResult;

	pTransform->Set_State(CTransform::STATE::STATE_POSITION, vResult);
	
	Vec3 vDir = vResult - vCurrPos;
	vDir.Normalize();
	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);


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

void CState_Naruto_RunLoop::Push_Spline()
{
	Vec3 vServerPos(m_pPlayer->Get_TargetMatrix().m[3]);
	SplinePos Spline;
	Spline.vPos = vServerPos;
	Spline.vTargetPos = m_pPlayer->Get_TargetPos();
	m_Spline.push_back(Spline);
}

void CState_Naruto_RunLoop::Free()
{
	__super::Free();
}
