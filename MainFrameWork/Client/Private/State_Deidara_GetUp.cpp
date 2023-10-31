#include "stdafx.h"
#include "State_Deidara_GetUp.h"
#include "Boss_Deidara.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Deidara_GetUp::CState_Deidara_GetUp(const wstring& strStateName, CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_GetUp::CState_Deidara_GetUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Deidara_GetUp::Initialize()
{
	m_iDownLoop = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Down_Loop", 1.0f);
	m_iGetUp = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"GetUp", 1.0f);
		
	if (m_iDownLoop == -1)
		return E_FAIL;

	if (m_iGetUp == -1)
		return E_FAIL;


	return S_OK;
}

void CState_Deidara_GetUp::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iDownLoop, 0.1f, 0, 0);
	m_bLoopEnd = false;
}

void CState_Deidara_GetUp::Tick_State(_float fTimeDelta)
{
	CModel* pModel = m_pBoss->Get_ModelCom();
	_uint iAnimIndex = pModel->Get_CurrAnim();


	if (m_bLoopEnd == false && iAnimIndex == m_iDownLoop)
	{
		if (pModel->Is_AnimationEnd(m_iDownLoop))
		{
			m_pBoss->Reserve_Animation(m_iGetUp, 0.1f, 0, 0);
			m_bLoopEnd = true;
		}
	}

	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);


}

void CState_Deidara_GetUp::Exit_State()
{

}


void CState_Deidara_GetUp::Free()
{
	__super::Free();
}
