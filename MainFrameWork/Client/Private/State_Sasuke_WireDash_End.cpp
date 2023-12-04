#include "stdafx.h"
#include "State_Sasuke_WireDash_End.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"
#include "WireTrail.h"


CState_Sasuke_WireDash_End::CState_Sasuke_WireDash_End(const wstring& strStateName, class CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_WireDash_End::CState_Sasuke_WireDash_End(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_WireDash_End::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Aerial_Dash_End", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_WireDash_End::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_WireDash_End::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_WireDash_End::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_Wall(true);
	m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Set_Gravity(false);
	m_pPlayer->Get_RigidBody()->UseGravity(false);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(false);
	m_pPlayer->Get_RigidBody()->Set_Ground(true);
	m_pPlayer->Get_RigidBody()->SetLinearVelocity(Vec3(0.0f, 0.0f, 0.0f));
	m_pPlayer->Appear_FootTrail();
	m_pPlayer->Get_WireTrail()->Disappear();



	m_vPrevPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
}

void CState_Sasuke_WireDash_End::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_WireDash_End::Exit_State()
{
	m_pPlayer->Get_WireTrail()->Set_Active(false);
}

void CState_Sasuke_WireDash_End::Tick_State_Control(_float fTimeDelta)
{
	//if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iLand))
	//	m_pPlayer->Set_State(L"Idle");

	CTransform* pTransform = m_pPlayer->Get_TransformCom();


	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();

	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);



	{
		_float fMove = (vPos - m_vPrevPos).Length();
		if (fMove > 0.0001f)
		{
			Vec3 vUp = pTransform->Get_State(CTransform::STATE_UP);
			vUp.Normalize();

			TRIAGLEDESC tTriangle = m_pPlayer->Get_Triangle();

			if (vUp.Dot(tTriangle.vNormal) >= 0.95f)
			{
				m_pPlayer->Set_State(L"Idle");
				m_pPlayer->Set_Picking(true);
			}
			else
			{
				vUp = Vec3::Lerp(vUp, tTriangle.vNormal, 8.0f * fTimeDelta);
				pTransform->Set_Up(vUp);
			}
			return;
		}
	}



	Vec3 vDistance = vTargetPos - vPos;
	Vec3 vDir = vDistance;
	vDir.Normalize();


	Vec3 vMove = vDir * 15.0f * fTimeDelta;
	

	if (vDistance.Length() <= vMove.Length())
	{
		m_pPlayer->Set_State(L"Idle");
		m_pPlayer->Set_Picking(true);
	}
	else
		vPos += vMove;

	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
	
	pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_vPrevPos = vPos;


	TRIAGLEDESC tTriangle = m_pPlayer->Get_Triangle();
	Vec3 vUp = pTransform->Get_State(CTransform::STATE_UP);
	vUp.Normalize();
	vUp = Vec3::Lerp(vUp, tTriangle.vNormal, 8.0f * fTimeDelta);

	pTransform->Set_Up(vUp);

}

void CState_Sasuke_WireDash_End::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_WireDash_End::Free()
{
	__super::Free();
}
