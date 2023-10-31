#include "stdafx.h"
#include "State_Deidara_FallBehind.h"
#include "Boss_Deidara.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Deidara_FallBehind::CState_Deidara_FallBehind(const wstring& strStateName, CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_FallBehind::CState_Deidara_FallBehind(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Deidara_FallBehind::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Fall_Behind_Loop", 1.0f);


	if (m_iAnimIndex == -1)
		return E_FAIL;

	return S_OK;
}

void CState_Deidara_FallBehind::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
}

void CState_Deidara_FallBehind::Tick_State(_float fTimeDelta)
{
	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_FallBehind::Exit_State()
{

}

void CState_Deidara_FallBehind::Free()
{
	__super::Free();
}
