#include "stdafx.h"
#include "State_Naruto_FallFront.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "PickingMgr.h"

CState_Naruto_FallFront::CState_Naruto_FallFront(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_FallFront::CState_Naruto_FallFront(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_FallFront::Initialize()
{
	m_iFall_Front = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Fall_Front_Loop", 1.0f);

	if (m_iFall_Front == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_FallFront::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_FallFront::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_FallFront::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iFall_Front, 0.2f, 0, 0);
	m_pPlayer->Get_RigidBody()->UseGravity(true);
	m_pPlayer->Set_Render(true);
	m_pPlayer->Reset_Triangle();
}

void CState_Naruto_FallFront::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_FallFront::Exit_State()
{
	m_pPlayer->Set_Picking(false);
}

void CState_Naruto_FallFront::Tick_State_Control(_float fTimeDelta)
{
	if (KEY_TAP(KEY::SPACE) && m_pPlayer->Is_DoubleJump() == false)
	{
		m_pPlayer->Set_State(L"DoubleJump");
		return;
	}

	if (m_pPlayer->Get_RigidBody()->Is_Gruond() == true)
	{
		m_pPlayer->Set_State(L"Land");
		return;
	}


	if (KEY_TAP(KEY::NUM_1))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENGAN))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENGAN, false);
			m_pPlayer->Set_State(L"AerialRasengun_Start");
			return;
		}
	}


	if (KEY_TAP(KEY::NUM_2))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENSYURIKEN))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENSYURIKEN, false);
			m_pPlayer->Set_State(L"AerialRasenSyuriken");
			return;
		}

	}


	if (KEY_TAP(KEY::CTRL))
	{
		m_pPlayer->Set_State(L"WireStart");
		return;
	}


	Set_Ray();

	Set_Wall();
}

void CState_Naruto_FallFront::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_FallFront::Set_Ray()
{
	Vec3 vDir(0.0f, 0.0f, 0.0f);
	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);


	if (KEY_HOLD(KEY::W))
	{
		vDir += m_pPlayer->Make_Straight_JumpDir();
	}

	if (KEY_HOLD(KEY::S))
	{
		vDir += m_pPlayer->Make_Back_JumpDir();
	}

	if (KEY_HOLD(KEY::A))
	{
		vDir += m_pPlayer->Make_Left_JumpDir();
	}

	if (KEY_HOLD(KEY::D))
	{
		vDir += m_pPlayer->Make_Right_JumpDir();
	}

	if (vDir.Length() > 0.0f)
	{
		vDir.Normalize();
		vPos.y += 0.4f;
		vPos -= vDir * 0.5f;

		CPickingMgr::GetInstance()->Set_Ray(vPos, vDir);
		m_pPlayer->Set_Picking(true);
	}
	else
	{
		m_pPlayer->Set_Picking(false);
	}
}

void CState_Naruto_FallFront::Set_Wall()
{
	if (!m_pPlayer->Is_Picking())
		return;



	TRIAGLEDESC tTriangle = m_pPlayer->Get_Triangle();

	if (tTriangle.fDist == -1.0f)
		return;

	Vec3 vRayPos, vRayDir;

	CPickingMgr::GetInstance()->Get_Ray(vRayPos, vRayDir);

	_float fDist;

	if (TriangleTests::Intersects(vRayPos, vRayDir, tTriangle.vPos0, tTriangle.vPos1, tTriangle.vPos2, fDist))
	{
		Vec3 vPickPos = vRayPos + vRayDir * fDist;

		Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		Vec3 vDistance = vPickPos - vPlayerPos;

		if (vDistance.Length() < 2.0f)
		{
			m_pPlayer->Set_Picking(false);

			m_pPlayer->Set_TargetPos(vPickPos);

			m_pPlayer->Set_State(L"WallLand");
		}
	}
}

void CState_Naruto_FallFront::Free()
{
	__super::Free();
}
