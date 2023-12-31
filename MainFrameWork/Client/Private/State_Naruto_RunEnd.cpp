#include "stdafx.h"
#include "State_Naruto_RunEnd.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "StateMachine.h"

CState_Naruto_RunEnd::CState_Naruto_RunEnd(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	: CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_RunEnd::CState_Naruto_RunEnd(const CState& rhs, CStateMachine* pMachine)
	: CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_RunEnd::Initialize()
{
	m_iRun_End = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Run_End", 1.0f);

	if (m_iRun_End == -1)
		return E_FAIL;

	m_vStopPos = Vec3(0.0f, 0.0f, 0.0f);

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_RunEnd::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_RunEnd::Tick_State_NoneControl;



	return S_OK;
}

void CState_Naruto_RunEnd::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iRun_End, 0.1f, 0, 0);

	if (m_pPlayer->Is_Control())
	{
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION);
		Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_LOOK);
		vLook.Normalize();

		m_vStopPos = vPos + vLook * (m_pPlayer->Get_MoveSpeed() * 0.2f);
		m_pPlayer->Set_TargetPos(m_vStopPos);
	}
	else
		m_vStopPos = m_pPlayer->Get_TargetPos();

	m_pPlayer->Set_MoveSpeed(0.0f);


	if (m_pPlayer->Is_Wall())
		m_pPlayer->DisAppear_FootTrail();


	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"MoveStop.wav", g_fVolume * 0.4f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
}

void CState_Naruto_RunEnd::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_RunEnd::Exit_State()
{
	m_vStopPos = Vec3(0.0f, 0.0f, 0.0f);
}

void CState_Naruto_RunEnd::Tick_State_Control(_float fTimeDelta)
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
		vPos = Vec3::Lerp(vPos, m_vStopPos, 4.0f * fTimeDelta);
		pTransform->Set_State(CTransform::STATE::STATE_POSITION, vPos);
	}

	if (KEY_TAP(KEY::NUM_1))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENGAN))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENGAN, false);
			m_pPlayer->Set_State(L"Rasengun_Start");
			return;
		}
	}

	if (KEY_TAP(KEY::NUM_2))
	{
		if (m_pPlayer->Get_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENSYURIKEN))
		{
			m_pPlayer->Set_SkillReady(CPlayer_Naruto::NARUTO_SKILL::RASENSYURIKEN, false);
			m_pPlayer->Set_State(L"RasenSyuriken");
			return;
		}
	}


	if (KEY_HOLD(KEY::W) || KEY_HOLD(KEY::A) || KEY_HOLD(KEY::S) || KEY_HOLD(KEY::D))
	{
		m_pPlayer->Set_State(L"Walk_Loop");
		return;
	}

	if (KEY_TAP(KEY::LBTN))
	{
		m_pPlayer->Set_State(L"Attack_Normal_cmb01");
		return;
	}
}

void CState_Naruto_RunEnd::Tick_State_NoneControl(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();
	Vec3 vCurrPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
	Vec3 vDir(m_pPlayer->Get_TargetMatrix().m[2]);

	vDir.Normalize();

	vCurrPos = Vec3::Lerp(vCurrPos, vTargetPos, 0.05f);
	pTransform->Set_State(CTransform::STATE::STATE_POSITION, vCurrPos);
	pTransform->LookAt_Lerp(vDir, 5.0f, fTimeDelta);
}



void CState_Naruto_RunEnd::Free()
{
	__super::Free();
}
