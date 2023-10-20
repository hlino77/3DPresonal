#include "stdafx.h"
#include "State_WhiteZetsu_ChasePlayer.h"
#include "Monster_WhiteZetsu.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_ChasePlayer::CState_WhiteZetsu_ChasePlayer(const wstring& strStateName, class CMonster_WhiteZetsu* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_ChasePlayer::CState_WhiteZetsu_ChasePlayer(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_ChasePlayer::Initialize()
{
	m_iRun_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 0.8f);
	m_pMonster->Get_ModelCom()->Set_CurrAnim(m_iRun_Loop);
	if (m_iRun_Loop == -1)
		return E_FAIL;


	m_fAccel = 10.0f;
	m_fMaxSpeed = 4.0f;

	return S_OK;
}

void CState_WhiteZetsu_ChasePlayer::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iRun_Loop, 0.1f, 0, 0);
}

void CState_WhiteZetsu_ChasePlayer::Tick_State(_float fTimeDelta)
{
	CTransform* pTransform = m_pMonster->Get_TransformCom();
	Vec3 vTargetPos = m_pMonster->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);

	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	
	vDir.Normalize();

	_float fCurrSpeed = m_pMonster->Get_MoveSpeed();

	if (fCurrSpeed < m_fMaxSpeed)
	{
		fCurrSpeed += m_fAccel * fTimeDelta;
		fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
		m_pMonster->Set_MoveSpeed(fCurrSpeed);
	}
	m_pMonster->Move_Dir(vDir, fCurrSpeed, fTimeDelta);


	m_pMonster->Follow_ServerPos(0.01f, 0.1f);
}

void CState_WhiteZetsu_ChasePlayer::Exit_State()
{

}


void CState_WhiteZetsu_ChasePlayer::Free()
{
	__super::Free();
}
