#include "stdafx.h"
#include "State_Naruto_Jump.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Naruto_Jump::CState_Naruto_Jump(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Jump::CState_Naruto_Jump(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Jump::Initialize()
{
	m_iJump_Front = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Jump_Front", 1.0f);
	m_iJump_Vertical = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Jump_Vertical", 4.0f);

	if (m_iJump_Front == -1)
		return E_FAIL;

	if (m_iJump_Vertical == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Jump::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Jump::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_Jump::Enter_State()
{
	m_pPlayer->Set_MoveSpeed(0.0f);

	if(m_pPlayer->Is_Control())
		Set_TargetPos();


	Set_Jump();

	
	m_pPlayer->Get_RigidBody()->UseGravity(false);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(false);
	m_pPlayer->Set_DoubleJump(false);
	m_pPlayer->Set_Wall(false);
}

void CState_Naruto_Jump::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Jump::Exit_State()
{
	m_pPlayer->Get_RigidBody()->SetCompareGruond(true);
}

void CState_Naruto_Jump::Tick_State_Control(_float fTimeDelta)
{
	
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();

	if(vTargetPos.y <= vPos.y)
		m_pPlayer->Set_State(L"Fall_Front");
		
}

void CState_Naruto_Jump::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Jump::Set_TargetPos()
{
	Vec3 vDir(0.0f, 0.0f, 0.0f);

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

	Vec3 vPlayerPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vTargetPos = vPlayerPos;

	if (vDir.Length() > 0.0f)
	{
		vDir.Normalize();

		vTargetPos += (vDir * 2.0f);
		vTargetPos.y += 2.5f;
	}
	else
	{
		vTargetPos = vPlayerPos;
		vTargetPos.y += 2.5f;
	}


	m_pPlayer->Set_TargetPos(vTargetPos);
}

void CState_Naruto_Jump::Set_Jump()
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();


	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();

	_float fAngle = XMConvertToDegrees(acosf(vDir.Dot(Vec3(0.0f, 1.0f, 0.0f))));

	if (fAngle > 10.0f)
	{
		Vec3 vAt = vPos + vDir;
		pTransform->LookAt_ForLandObject(vAt);
		m_pPlayer->Set_Render(false);

		vDir *= 15.0f;
		m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::VELOCITY_CHANGE);
		m_pPlayer->Get_RigidBody()->AddForce(vDir, ForceMode::FORCE);
		m_pPlayer->Reserve_Animation(m_iJump_Front, 0.1f, 0, 0);
	}
	else
	{
		vDir *= 15.0f;
		m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::VELOCITY_CHANGE);
		m_pPlayer->Get_RigidBody()->AddForce(vDir, ForceMode::FORCE);
		m_pPlayer->Reserve_Animation(m_iJump_Vertical, 0.1f, 0, 0);
	}
}



void CState_Naruto_Jump::Free()
{
	__super::Free();
}
