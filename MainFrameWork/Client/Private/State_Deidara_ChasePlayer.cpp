#include "stdafx.h"
#include "State_Deidara_ChasePlayer.h"
#include "Boss_Deidara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_ChasePlayer::CState_Deidara_ChasePlayer(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_ChasePlayer::CState_Deidara_ChasePlayer(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_ChasePlayer::Initialize()
{
	m_iRun_Loop = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 1.2f);
	m_pBoss->Get_ModelCom()->Set_CurrAnim(m_iRun_Loop);
	if (m_iRun_Loop == -1)
		return E_FAIL;


	m_fAccel = 10.0f;
	m_fMaxSpeed = 8.0f;

	return S_OK;
}

void CState_Deidara_ChasePlayer::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iRun_Loop, 0.1f, 0, 0);
}

void CState_Deidara_ChasePlayer::Tick_State(_float fTimeDelta)
{
	CTransform* pTransform = m_pBoss->Get_TransformCom();
	Vec3 vTargetPos = m_pBoss->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);

	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	
	vDir.Normalize();

	_float fCurrSpeed = m_pBoss->Get_MoveSpeed();

	if (fCurrSpeed < m_fMaxSpeed)
	{
		fCurrSpeed += m_fAccel * fTimeDelta;
		fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
		m_pBoss->Set_MoveSpeed(fCurrSpeed);
	}
	m_pBoss->Move_Dir(vDir, fCurrSpeed, fTimeDelta);


	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);

	Update_MoveSound();
}

void CState_Deidara_ChasePlayer::Exit_State()
{
}

void CState_Deidara_ChasePlayer::Update_MoveSound()
{
	_uint iFrame = m_pBoss->Get_ModelCom()->Get_Anim_Frame(m_iRun_Loop);

	_uint iFirstFrame = 1;
	_uint iSecondFrame = 7;

	if (iFrame == iFirstFrame || iFrame == iFirstFrame + 1 || iFrame == iSecondFrame || iFrame == iSecondFrame + 1)
	{
		if (m_bMoveSound == false)
		{
			CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"Move_2.wav", g_fVolume * 0.2f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 25.0f);
			m_bMoveSound = true;
		}

	}
	else
		m_bMoveSound = false;
}


void CState_Deidara_ChasePlayer::Free()
{
	__super::Free();
}
