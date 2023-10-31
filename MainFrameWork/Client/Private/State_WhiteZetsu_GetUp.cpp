#include "stdafx.h"
#include "State_WhiteZetsu_GetUp.h"
#include "Monster_WhiteZetsu.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_WhiteZetsu_GetUp::CState_WhiteZetsu_GetUp(const wstring& strStateName, CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_GetUp::CState_WhiteZetsu_GetUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_WhiteZetsu_GetUp::Initialize()
{
	m_iDownLoop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Down_Loop", 1.0f);
	m_iGetUp = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"GetUp", 1.0f);

	


	if (m_iDownLoop == -1)
		return E_FAIL;

	if (m_iGetUp == -1)
		return E_FAIL;


	return S_OK;
}

void CState_WhiteZetsu_GetUp::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iDownLoop, 0.1f, 0, 0);
	m_bLoopEnd = false;
}

void CState_WhiteZetsu_GetUp::Tick_State(_float fTimeDelta)
{
	
	CModel* pModel = m_pMonster->Get_ModelCom();
	_uint iAnimIndex = pModel->Get_CurrAnim();


	if (iAnimIndex == m_iDownLoop)
	{
		if (m_bLoopEnd == false && pModel->Is_AnimationEnd(m_iDownLoop))
		{
			m_pMonster->Reserve_Animation(m_iGetUp, 0.1f, 0, 0);
			m_bLoopEnd = true;
		}
	}

	m_pMonster->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_WhiteZetsu_GetUp::Exit_State()
{

}


void CState_WhiteZetsu_GetUp::Free()
{
	__super::Free();
}
