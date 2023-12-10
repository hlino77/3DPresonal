#include "stdafx.h"
#include "State_Madara_Dying_Normal.h"
#include "Boss_Madara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Dying_Normal::CState_Madara_Dying_Normal(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Dying_Normal::CState_Madara_Dying_Normal(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Dying_Normal::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Dying_Type01", 1.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Dying_Normal::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pBoss->Set_Die();

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"Madara_Die.wav", g_fVolume * 0.6f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 50.0f);
}

void CState_Madara_Dying_Normal::Tick_State(_float fTimeDelta)
{
}

void CState_Madara_Dying_Normal::Exit_State()
{

}


void CState_Madara_Dying_Normal::Free()
{
	__super::Free();
}
