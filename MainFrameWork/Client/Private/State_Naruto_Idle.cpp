#include "stdafx.h"
#include "State_Naruto_Idle.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"

CState_Naruto_Idle::CState_Naruto_Idle(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Idle::CState_Naruto_Idle(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Idle::Initialize()
{
	m_iIdle_Loop = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Idle_Loop", 1.0f);
	m_pPlayer->Get_ModelCom()->Set_CurrAnim(m_iIdle_Loop);
	if (m_iIdle_Loop == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Idle::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Idle::Tick_State_NoneControl;

	return S_OK;
}

void CState_Naruto_Idle::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iIdle_Loop, 0.1f, 0, 0);
}

void CState_Naruto_Idle::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Idle::Exit_State()
{

}

void CState_Naruto_Idle::Tick_State_Control(_float fTimeDelta)
{
	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D) || KEY_HOLD(KEY::A))
	{
		m_pPlayer->Set_State(L"Run_Loop");
	}

	if (KEY_TAP(KEY::LBTN))
		m_pPlayer->Set_State(L"Attack_Normal_cmb01");
}

void CState_Naruto_Idle::Tick_State_NoneControl(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();
	Vec3 vCurrPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vServerPos(m_pPlayer->Get_TargetMatrix().m[3]);

	if ((vServerPos - vCurrPos).Length() > 0.1f)
	{
		vCurrPos = Vec3::Lerp(vCurrPos, vServerPos, 0.2f);
	}
}

void CState_Naruto_Idle::Free()
{
	__super::Free();
}
