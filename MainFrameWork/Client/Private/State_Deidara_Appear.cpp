#include "stdafx.h"
#include "State_Deidara_Appear.h"
#include "Boss_Deidara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Appear::CState_Deidara_Appear(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Appear::CState_Deidara_Appear(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Appear::Initialize()
{
	m_iAppear = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Appearance", 1.0f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iAppear);
	if (m_iAppear == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_Appear::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAppear, 0.1f, 0, 0);
}

void CState_Deidara_Appear::Tick_State(_float fTimeDelta)
{
	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_Appear::Exit_State()
{

}


void CState_Deidara_Appear::Free()
{
	__super::Free();
}
