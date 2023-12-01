#include "stdafx.h"
#include "State_Sasuke_Skill_Chidori_Start.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Skill_Chidori.h"


CState_Sasuke_Skill_Chidori_Start::CState_Sasuke_Skill_Chidori_Start(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Skill_Chidori_Start::CState_Sasuke_Skill_Chidori_Start(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Skill_Chidori_Start::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Chidori_Charge_Lv1", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Skill_Chidori_Start::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Skill_Chidori_Start::Tick_State_NoneControl;

	

	return S_OK;
}

void CState_Sasuke_Skill_Chidori_Start::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_Invincible(true);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"NinjutsuStart.wav", g_fVolume * 0.2f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
}

void CState_Sasuke_Skill_Chidori_Start::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_Start::Exit_State()
{

}

void CState_Sasuke_Skill_Chidori_Start::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame > 8)
	{
		m_pPlayer->Set_State(L"Chidori_Charge");
	}



	//Prototype_GameObject_RasenganCircle

	
}

void CState_Sasuke_Skill_Chidori_Start::Tick_State_NoneControl(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();
	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);


	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_Start::Free()
{
	__super::Free();
}
