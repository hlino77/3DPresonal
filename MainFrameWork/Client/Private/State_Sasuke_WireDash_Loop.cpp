#include "stdafx.h"
#include "State_Sasuke_WireDash_Loop.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Skill_Rasengun.h"
#include "RigidBody.h"

CState_Sasuke_WireDash_Loop::CState_Sasuke_WireDash_Loop(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_WireDash_Loop::CState_Sasuke_WireDash_Loop(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_WireDash_Loop::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Aerial_Dash_Loop", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_WireDash_Loop::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_WireDash_Loop::Tick_State_NoneControl;


	return S_OK;
}

void CState_Sasuke_WireDash_Loop::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_Invincible(true);
}

void CState_Sasuke_WireDash_Loop::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_WireDash_Loop::Exit_State()
{
	m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Set_Gravity(true);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(true);
	m_pPlayer->Set_DefaultUp(true);
}

void CState_Sasuke_WireDash_Loop::Tick_State_Control(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();


	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_float fDistance = (vTargetPos - vPos).Length();

	if (fDistance > 2.0f)
	{
		pTransform->LookAt(vTargetPos);
		pTransform->Go_Straight(25.0f, fTimeDelta);
	}
	else
	{
		m_pPlayer->Set_State(L"WireDash_End");
	}

	//Prototype_GameObject_RasenganCircle

}

void CState_Sasuke_WireDash_Loop::Tick_State_NoneControl(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();


	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	_float fDistance = (vTargetPos - vPos).Length();

	if (fDistance > 2.0f)
	{
		pTransform->LookAt(vTargetPos);
		pTransform->Go_Straight(25.0f, fTimeDelta);
	}


	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_WireDash_Loop::Free()
{
	__super::Free();
}
