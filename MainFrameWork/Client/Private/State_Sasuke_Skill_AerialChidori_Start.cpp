#include "stdafx.h"
#include "State_Sasuke_Skill_AerialChidori_Start.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Skill_Chidori.h"
#include "RigidBody.h"



CState_Sasuke_Skill_AerialChidori_Start::CState_Sasuke_Skill_AerialChidori_Start(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Skill_AerialChidori_Start::CState_Sasuke_Skill_AerialChidori_Start(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Skill_AerialChidori_Start::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Aerial_Ninjutsu_Chidori_Charge_Lv1", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Skill_AerialChidori_Start::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Skill_AerialChidori_Start::Tick_State_NoneControl;


	return S_OK;
}

void CState_Sasuke_Skill_AerialChidori_Start::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_Invincible(true);


	m_pPlayer->Set_Gravity(false);
	m_pPlayer->Set_DefaultUp(false);
	m_pPlayer->Get_RigidBody()->UseDrag(false);
	m_pPlayer->Get_RigidBody()->UseGravity(false);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(false);
	m_pPlayer->Get_RigidBody()->ClearForce(ForceMode::VELOCITY_CHANGE);
}

void CState_Sasuke_Skill_AerialChidori_Start::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Skill_AerialChidori_Start::Exit_State()
{

}

void CState_Sasuke_Skill_AerialChidori_Start::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame > 8)
	{
		m_pPlayer->Set_State(L"AerialChidori_Charge");
	}



	//Prototype_GameObject_RasenganCircle

	
}

void CState_Sasuke_Skill_AerialChidori_Start::Tick_State_NoneControl(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Skill_AerialChidori_Start::Free()
{
	__super::Free();
}
