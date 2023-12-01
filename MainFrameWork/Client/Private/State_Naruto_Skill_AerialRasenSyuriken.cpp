#include "stdafx.h"
#include "State_Naruto_Skill_AerialRasenSyuriken.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Skill_RasenSyuriken.h"
#include "RigidBody.h"


CState_Naruto_Skill_AerialRasenSyuriken::CState_Naruto_Skill_AerialRasenSyuriken(const wstring& strStateName, class CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Skill_AerialRasenSyuriken::CState_Naruto_Skill_AerialRasenSyuriken(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Skill_AerialRasenSyuriken::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Aerial_TrueRasenShuriken1", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;


	if (m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Skill_AerialRasenSyuriken::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Skill_AerialRasenSyuriken::Tick_State_NoneControl;


	m_iAppearFrame = 9;
	m_iShootFrame = 70;


	

	return S_OK;
}

void CState_Naruto_Skill_AerialRasenSyuriken::Enter_State()
{
	if(m_pPlayer->Is_Control())
		Set_Target();

	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bShoot = false;
	m_bAppear = false;

	m_pPlayer->Set_Invincible(true);


	m_pPlayer->Set_Gravity(false);
	m_pPlayer->Set_DefaultUp(false);
	m_pPlayer->Get_RigidBody()->UseDrag(false);
	m_pPlayer->Get_RigidBody()->UseGravity(false);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(false);
	m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::VELOCITY_CHANGE);


	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"NinjutsuStart.wav", g_fVolume * 0.2f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
}

void CState_Naruto_Skill_AerialRasenSyuriken::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Skill_AerialRasenSyuriken::Exit_State()
{
	m_pPlayer->Set_Invincible(false);


	m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Set_Gravity(true);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(true);
	m_pPlayer->Set_DefaultUp(true);
}

void CState_Naruto_Skill_AerialRasenSyuriken::Tick_State_Control(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame < m_iShootFrame)
	{
		LookAt_Target(fTimeDelta);
	}


	if (iFrame > m_iAppearFrame && iFrame < m_iShootFrame)
	{
		if (m_bAppear == false)
		{
			m_pPlayer->Get_RasenSyuriken()->Appear();
			m_bAppear = true;
		}
	}
	else if (iFrame > m_iShootFrame)
	{
		if (m_bShoot == false)
		{
			m_pPlayer->Get_RasenSyuriken()->Shoot();
			m_bShoot = true;
		}
	}

	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"Fall_Front");


}

void CState_Naruto_Skill_AerialRasenSyuriken::Tick_State_NoneControl(_float fTimeDelta)
{
	_uint iFrame = m_pPlayer->Get_ModelCom()->Get_Anim_Frame(m_iAnimIndex);

	if (iFrame < m_iShootFrame)
	{
		LookAt_Target(fTimeDelta);
	}


	if (iFrame > m_iAppearFrame && iFrame < m_iShootFrame)
	{
		if (m_bAppear == false)
		{
			m_pPlayer->Get_RasenSyuriken()->Appear();
			m_bAppear = true;
		}
	}
	else if (iFrame > m_iShootFrame)
	{
		if (m_bShoot == false)
		{
			m_pPlayer->Get_RasenSyuriken()->Shoot();
			m_bShoot = true;
		}
	}


	
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Skill_AerialRasenSyuriken::Set_Target()
{
	m_pPlayer->Find_NearTarget();

	CGameObject* pNearTarget = m_pPlayer->Get_NearTarget();

	if (pNearTarget)
	{
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_float fDistance = (vTargetPos - vPos).Length();

		if (fDistance > 20.0f)
		{
			pNearTarget = nullptr;
			m_pPlayer->Set_NearTarget(nullptr);
		}	
	}

	m_pPlayer->Send_NearTarget();

	if (pNearTarget == nullptr)
	{
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		vLook.Normalize();
		Vec3 vTargetPos = vPos + (vLook * 2.0f);
		m_pPlayer->Set_TargetPos(vTargetPos);
	}
}

void CState_Naruto_Skill_AerialRasenSyuriken::LookAt_Target(_float fTimeDelta)
{
	CGameObject* pNearTarget = m_pPlayer->Get_NearTarget();


	Vec3 vTargetPos;
	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	if (pNearTarget == nullptr)
	{
		vTargetPos = m_pPlayer->Get_TargetPos();
	}
	else
	{
		vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	}


	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();


	m_pPlayer->Get_TransformCom()->SetUp_Lerp(vDir, 10.0f, fTimeDelta);
	m_pPlayer->Get_TransformCom()->LookAt_Lerp_ForLand(vDir, 10.0f, fTimeDelta);
	//m_pPlayer->Get_TransformCom()->LookAt_Lerp(vDir, 8.0f, fTimeDelta);
}

void CState_Naruto_Skill_AerialRasenSyuriken::Free()
{
	__super::Free();
}
