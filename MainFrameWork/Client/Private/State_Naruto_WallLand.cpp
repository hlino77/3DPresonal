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

	m_pPlayer->Get_RigidBody()->UseGravity(false);


	
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


	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();

	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);




	Vec3 vDistance = vTargetPos - vPos;
	Vec3 vDir = vDistance;
	vDir.Normalize();


	Vec3 vMove = vDir * 10.0f * fTimeDelta;
	

	if (vDistance.Length() <= vMove.Length())
		vPos = vTargetPos;
	else
		vPos += vMove;

	m_pPlayer->Get_TransformCom()->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
	
	m_pPlayer->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);


	if (vPos == vTargetPos)
	{

		TRIAGLEDESC tTriangle = m_pPlayer->Get_Triangle();

		Vec3 vDir1 = tTriangle.vPos1 - tTriangle.vPos0;
		Vec3 vDir2 = tTriangle.vPos2 - tTriangle.vPos1;

		Vec3 vNormal = vDir1.Cross(vDir2);
		vNormal.Normalize();

		m_pPlayer->Get_TransformCom()->Set_Up(vNormal);

		m_pPlayer->Set_Wall(true);
		m_pPlayer->Set_State(L"Idle");
		
		m_pPlayer->Set_Picking(true);
	}
}

void CState_Naruto_WallLand::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 0.1f);
}

void CState_Naruto_WallLand::Free()
{
	__super::Free();
}
