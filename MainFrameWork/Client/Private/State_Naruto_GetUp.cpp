#include "stdafx.h"
#include "State_Naruto_GetUp.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "RigidBody.h"

CState_Naruto_GetUp::CState_Naruto_GetUp(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_GetUp::CState_Naruto_GetUp(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_GetUp::Initialize()
{
	m_iDownLoop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Beaten_Down_Loop", 1.0f);
	m_iGetUp = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"GetUp", 1.0f);

	


	if (m_iDownLoop == -1)
		return E_FAIL;

	if (m_iGetUp == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_GetUp::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_GetUp::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_GetUp::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iDownLoop, 0.1f, 0, 0);
	m_bLoopEnd = false;
}

void CState_Naruto_GetUp::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_GetUp::Exit_State()
{

}

void CState_Naruto_GetUp::Tick_State_Control(_float fTimeDelta)
{
	CModel* pModel = m_pPlayer->Get_ModelCom();
	_uint iAnimIndex = pModel->Get_CurrAnim();


	if (m_bLoopEnd == false && iAnimIndex == m_iDownLoop)
	{
		if (pModel->Is_AnimationEnd(m_iDownLoop))
		{
			m_pPlayer->Reserve_Animation(m_iGetUp, 0.1f, 0, 0);
			m_bLoopEnd = true;
		}
	}

	if (iAnimIndex == m_iGetUp)
	{
		if (pModel->Is_AnimationEnd(m_iGetUp))
			m_pPlayer->Set_State(L"Idle");
	}


}

void CState_Naruto_GetUp::Tick_State_NoneControl(_float fTimeDelta)
{
	CModel* pModel = m_pPlayer->Get_ModelCom();
	_uint iAnimIndex = pModel->Get_CurrAnim();


	if (iAnimIndex == m_iDownLoop)
	{
		if (m_bLoopEnd == false && pModel->Is_AnimationEnd(m_iDownLoop))
		{
			m_pPlayer->Reserve_Animation(m_iGetUp, 0.1f, 1, 0);
			m_bLoopEnd = true;
		}
	}
}



void CState_Naruto_GetUp::Free()
{
	__super::Free();
}
