#include "stdafx.h"
#include "State_Sasuke_RunEnd.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Sasuke_RunEnd::CState_Sasuke_RunEnd(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	: CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_RunEnd::CState_Sasuke_RunEnd(const CState& rhs, CStateMachine* pMachine)
	: CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_RunEnd::Initialize()
{
	m_iRun_End = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Run_End", 1.0f);

	if (m_iRun_End == -1)
		return E_FAIL;

	m_vStopPos = Vec3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

void CState_Sasuke_RunEnd::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iRun_End, 0.1f, 0, 0);

	Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
	vLook.Normalize();

	m_vStopPos = vPos + vLook * 1.0f;
}

void CState_Sasuke_RunEnd::Tick_State(_float fTimeDelta)
{
	if (m_pPlayer->Get_ModelCom()->Is_AnimationEnd(m_iRun_End))
	{
		m_pPlayer->Set_State(L"Idle");
		return;
	}
	else
	{
		CTransform* pTransform = m_pPlayer->Get_TransformCom();
		Vec3 vPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
		vPos = Vec3::Lerp(vPos, m_vStopPos, 0.1f);
		pTransform->Set_State(CTransform::STATE::STATE_POSITION, vPos);
	}

	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D))
	{
		m_pPlayer->Set_State(L"Run_Loop");
	}
}

void CState_Sasuke_RunEnd::Exit_State()
{
	m_vStopPos = Vec3(0.0f, 0.0f, 0.0f);
}



void CState_Sasuke_RunEnd::Free()
{
	__super::Free();
}
