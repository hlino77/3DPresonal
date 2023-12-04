#include "stdafx.h"
#include "State_Sasuke_DownToFloor.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Sasuke_DownToFloor::CState_Sasuke_DownToFloor(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_DownToFloor::CState_Sasuke_DownToFloor(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_DownToFloor::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_DownToFloor", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_DownToFloor::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_DownToFloor::Tick_State_NoneControl;

	return S_OK;
}

void CState_Sasuke_DownToFloor::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"DownToFloor.wav", g_fVolume * 0.15f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
}

void CState_Sasuke_DownToFloor::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_DownToFloor::Exit_State()
{

}

void CState_Sasuke_DownToFloor::Tick_State_Control(_float fTimeDelta)
{
	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iAnimIndex))
		m_pPlayer->Set_State(L"GetUp");
}

void CState_Sasuke_DownToFloor::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}



void CState_Sasuke_DownToFloor::Free()
{
	__super::Free();
}
