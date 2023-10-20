#include "stdafx.h"
#include "State_WhiteZetsu_ChasePlayer_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Server_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_WhiteZetsu_ChasePlayer_Server::CState_WhiteZetsu_ChasePlayer_Server(const wstring& strStateName, class CMonster_WhiteZetsu_Server* pMonster)
	:CState(strStateName)
	, m_pMonster(pMonster)
{
}

CState_WhiteZetsu_ChasePlayer_Server::CState_WhiteZetsu_ChasePlayer_Server(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_WhiteZetsu_ChasePlayer_Server::Initialize()
{
	m_iRun_Loop = m_pMonster->Get_ModelCom()->Initailize_FindAnimation(L"Run_Loop", 0.8f);


	if (m_iRun_Loop == -1)
		return E_FAIL;


	m_fAccel = 10.0f;
	m_fMaxSpeed = 4.0f;

	return S_OK;
}

void CState_WhiteZetsu_ChasePlayer_Server::Enter_State()
{
	m_pMonster->Reserve_Animation(m_iRun_Loop, 0.1f, 0, 0);
}

void CState_WhiteZetsu_ChasePlayer_Server::Tick_State(_float fTimeDelta)
{
	m_pMonster->Update_NearTarget(fTimeDelta);

	

	CTransform* pTransform = m_pMonster->Get_TransformCom();
	Vec3 vTargetPos = m_pMonster->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);

	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	
	vDir.Normalize();

	if (fDistance > m_pMonster->Get_FollowDistance())
		m_pMonster->Set_State(L"Idle");
	else if (fDistance <= 1.0f)
		m_pMonster->Attack_Random();
	else
	{
		_float fCurrSpeed = m_pMonster->Get_MoveSpeed();

		if (fCurrSpeed < m_fMaxSpeed)
		{
			fCurrSpeed += m_fAccel * fTimeDelta;
			fCurrSpeed = min(fCurrSpeed, m_fMaxSpeed);
			m_pMonster->Set_MoveSpeed(fCurrSpeed);
		}
		m_pMonster->Move_Dir(vDir, fCurrSpeed, fTimeDelta);
	}

}

void CState_WhiteZetsu_ChasePlayer_Server::Exit_State()
{

}


void CState_WhiteZetsu_ChasePlayer_Server::Free()
{
	__super::Free();
}
