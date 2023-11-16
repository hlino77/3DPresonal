#include "stdafx.h"
#include "State_Naruto_Skill_Rasengun_Charging.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "LineCircle.h"
#include "RasenganCircle.h"

CState_Naruto_Skill_Rasengun_Charging::CState_Naruto_Skill_Rasengun_Charging(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Skill_Rasengun_Charging::CState_Naruto_Skill_Rasengun_Charging(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Skill_Rasengun_Charging::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Rasengun_Charge_Lv2toLv3", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Skill_Rasengun_Charging::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Skill_Rasengun_Charging::Tick_State_NoneControl;


	return S_OK;
}

void CState_Naruto_Skill_Rasengun_Charging::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Naruto_Skill_Rasengun_Charging::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_Charging::Exit_State()
{

}

void CState_Naruto_Skill_Rasengun_Charging::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();



	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
	{
		if (KEY_HOLD(KEY::NUM_1))
		{
			m_pPlayer->Set_State(L"Rasengun_Loop");
		}
		else
		{
			m_pPlayer->Set_State(L"Rasengun_RunStart");
		}
	}


	//Prototype_GameObject_RasenganCircle

	
}

void CState_Naruto_Skill_Rasengun_Charging::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_Charging::Free()
{
	__super::Free();
}
