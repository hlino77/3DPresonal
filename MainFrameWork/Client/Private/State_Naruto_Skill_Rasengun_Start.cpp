#include "stdafx.h"
#include "State_Naruto_Skill_Rasengun_Start.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Skill_Rasengun.h"


CState_Naruto_Skill_Rasengun_Start::CState_Naruto_Skill_Rasengun_Start(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Skill_Rasengun_Start::CState_Naruto_Skill_Rasengun_Start(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Skill_Rasengun_Start::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Rasengun_Charge_Lv1", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Skill_Rasengun_Start::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Skill_Rasengun_Start::Tick_State_NoneControl;


	return S_OK;
}

void CState_Naruto_Skill_Rasengun_Start::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_Invincible(true);
}

void CState_Naruto_Skill_Rasengun_Start::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_Start::Exit_State()
{

}

void CState_Naruto_Skill_Rasengun_Start::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame > 8)
	{
		m_pPlayer->Get_Rasengun()->Appear();
		m_pPlayer->Set_State(L"Rasengun_Charge");
	}



	//Prototype_GameObject_RasenganCircle

	
}

void CState_Naruto_Skill_Rasengun_Start::Tick_State_NoneControl(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame > 8)
	{
		m_pPlayer->Get_Rasengun()->Appear();
	}


	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_Start::Free()
{
	__super::Free();
}
