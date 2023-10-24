#include "stdafx.h"
#include "State_Naruto_DoubleJump.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Naruto_DoubleJump::CState_Naruto_DoubleJump(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_DoubleJump::CState_Naruto_DoubleJump(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_DoubleJump::Initialize()
{
	m_iDouble_Jump = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"DoubleJump", 3.0f);

	if (m_iDouble_Jump == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_DoubleJump::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_DoubleJump::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_DoubleJump::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iDouble_Jump, 0.1f, 0, 0);

	m_bLookAt = false;


	if (m_pPlayer->Is_Control())
		Set_TargetPos();
	else
	{
		Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vDir = vTargetPos - vPos;
		vDir.Normalize();

		_float fTargetPosAngle = XMConvertToDegrees(acosf(vDir.Dot(Vec3(0.0f, 1.0f, 0.0f))));
		if (fTargetPosAngle > 10.0f)
			m_bLookAt = true;
		else
			m_bLookAt = false;
	}

	Set_Jump();


	m_pPlayer->Set_DoubleJump(true);
}

void CState_Naruto_DoubleJump::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_DoubleJump::Exit_State()
{

}

void CState_Naruto_DoubleJump::Tick_State_Control(_float fTimeDelta)
{
	if (m_bLookAt)
		m_pPlayer->Get_TransformCom()->LookAt_Lerp(m_vDir, 10.0f, fTimeDelta);

	_uint iFrame = m_pPlayer->Get_ModelCom()->Get_Anim_Frame(m_iDouble_Jump);
	if (iFrame >= 44)
		m_pPlayer->Set_State(L"Fall_Front");		
}

void CState_Naruto_DoubleJump::Tick_State_NoneControl(_float fTimeDelta)
{
	if (m_bLookAt)
		m_pPlayer->Get_TransformCom()->LookAt_Lerp(m_vDir, 10.0f, fTimeDelta);

	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_DoubleJump::Set_TargetPos()
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

		vTargetPos += (vDir * 10.0f);
		vTargetPos.y += 15.0f;
		m_bLookAt = true;
	}
	else
	{
		vTargetPos = vPlayerPos;
		vTargetPos.y += 15.0f;
		m_bLookAt = false;
	}

	m_pPlayer->Set_TargetPos(vTargetPos);
}

void CState_Naruto_DoubleJump::Set_Jump()
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();


	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();

	vDir *= 20.0f;
	m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::VELOCITY_CHANGE);
	m_pPlayer->Get_RigidBody()->AddForce(vDir, ForceMode::FORCE);


	m_vDir = vDir;
	m_vDir.Normalize();
}

void CState_Naruto_DoubleJump::Free()
{
	__super::Free();
}
