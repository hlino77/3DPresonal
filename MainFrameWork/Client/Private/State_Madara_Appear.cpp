#include "stdafx.h"
#include "State_Madara_Appear.h"
#include "Boss_Madara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Appear::CState_Madara_Appear(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Appear::CState_Madara_Appear(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Appear::Initialize()
{
	m_iAppear = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Appear", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAppear);
	if (m_iAppear == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Madara_Appear::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAppear, 0.1f, 0, 0);
}

void CState_Madara_Appear::Tick_State(_float fTimeDelta)
{
	//m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_Appear::Exit_State()
{

}


void CState_Madara_Appear::Free()
{
	__super::Free();
}
