#include "stdafx.h"
#include "State_Naruto_WallLand.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"



CState_Naruto_WallLand::CState_Naruto_WallLand(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_WallLand::CState_Naruto_WallLand(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_WallLand::Initialize()
{
	m_iFallFront = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Fall_Front_Loop", 1.0f);

	if (m_iFallFront == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_WallLand::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_WallLand::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_WallLand::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iFallFront, 0.1f, 0, 0);

	m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Set_Gravity(false);
	m_pPlayer->Get_RigidBody()->UseGravity(false);
	m_pPlayer->Set_Wall(true);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(false);
	m_pPlayer->Get_RigidBody()->Set_Ground(true);
	m_pPlayer->Get_RigidBody()->SetLinearVelocity(Vec3(0.0f, 0.0f, 0.0f));
	m_pPlayer->Appear_FootTrail();


	m_vPrevPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
}

void CState_Naruto_WallLand::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_WallLand::Exit_State()
{
}

void CState_Naruto_WallLand::Tick_State_Control(_float fTimeDelta)
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

void CState_Naruto_WallLand::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_WallLand::Free()
{
	__super::Free();
}
