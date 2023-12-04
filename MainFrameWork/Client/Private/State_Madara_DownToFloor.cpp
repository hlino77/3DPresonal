#include "stdafx.h"
#include "State_Madara_DownToFloor.h"
#include "Boss_Madara.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Madara_DownToFloor::CState_Madara_DownToFloor(const wstring& strStateName, CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_DownToFloor::CState_Madara_DownToFloor(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Madara_DownToFloor::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_DownToFloor", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	
	return S_OK;
}

void CState_Madara_DownToFloor::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 2, 0);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"DownToFloor.wav", g_fVolume * 0.15f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
}

void CState_Madara_DownToFloor::Tick_State(_float fTimeDelta)
{		
	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_DownToFloor::Exit_State()
{

}



void CState_Madara_DownToFloor::Free()
{
	__super::Free();
}
