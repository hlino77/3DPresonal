#include "stdafx.h"
#include "State_Madara_Attack_DoubleTurnKick.h"
#include "Boss_Madara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Madara_Attack_DoubleTurnKick::CState_Madara_Attack_DoubleTurnKick(const wstring& strStateName, class CBoss_Madara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Madara_Attack_DoubleTurnKick::CState_Madara_Attack_DoubleTurnKick(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Madara_Attack_DoubleTurnKick::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Attack_DoubleTurnKick", 2.0f);

	if (m_iAnimIndex == -1)
		return E_FAIL;



	return S_OK;
}

void CState_Madara_Attack_DoubleTurnKick::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.2f, 0, 0);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(CGameInstance::GetInstance()->Get_RandomSoundKey(L"Madara_LargeAttack"), g_fVolume * 0.5f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Madara_Attack_DoubleTurnKick::Tick_State(_float fTimeDelta)
{
	CModel* pBossModel = m_pBoss->Get_ModelCom();

	_uint iFrame = pBossModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame >= 0 && iFrame < 7)
	{
		Follow_TargetPos(fTimeDelta);
	}


	if (iFrame >= 8 && iFrame < 13)
	{
		Follow_TargetPos(fTimeDelta);
	}




	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Madara_Attack_DoubleTurnKick::Exit_State()
{

}

void CState_Madara_Attack_DoubleTurnKick::Follow_TargetPos(_float fTimeDelta)
{
	_float fMoveSpeed = m_pBoss->Get_AttackMoveSpeed();

	if (fMoveSpeed <= 0.0f)
		return;

	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vBossPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pBoss->Get_NearTarget()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);


	vTargetPos.y = vBossPos.y;

	Vec3 vDir = vTargetPos - vBossPos;
	vDir.Normalize();

	m_pBoss->Get_TransformCom()->LookAt_Lerp(vDir, 10.0f, fTimeDelta);

	vTargetPos = vTargetPos + (vDir * -0.6f);
	vDir = vTargetPos - vBossPos;

	if (vDir.Length() < 0.001f)
		return;

	Vec3 vMove = vDir;
	vMove.Normalize();
	vMove *= fMoveSpeed * fTimeDelta;

	if (vDir.Length() > vMove.Length())
	{
		vBossPos += vMove;
		m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vBossPos);
	}
	else
	{
		vBossPos = vTargetPos;
		m_pBoss->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vBossPos);
	}
}


void CState_Madara_Attack_DoubleTurnKick::Free()
{
	__super::Free();
}
