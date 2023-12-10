#include "stdafx.h"
#include "State_Deidara_Attack_cmb02.h"
#include "Boss_Deidara.h"
#include "Client_Defines.h"
#include "Model.h"
#include "StateMachine.h"
#include "GameInstance.h"

CState_Deidara_Attack_cmb02::CState_Deidara_Attack_cmb02(const wstring& strStateName, class CBoss_Deidara* pBoss)
	:CState(strStateName)
	, m_pBoss(pBoss)
{
}

CState_Deidara_Attack_cmb02::CState_Deidara_Attack_cmb02(const CState& rhs, class CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}
HRESULT CState_Deidara_Attack_cmb02::Initialize()
{
	m_iAnimIndex = m_pBoss->Get_ModelCom()->Initailize_FindAnimation(L"Attack_cmb01", 1.2f);

	if (m_iAnimIndex == -1)
		return E_FAIL;



	return S_OK;
}

void CState_Deidara_Attack_cmb02::Enter_State()
{
	m_pBoss->Reserve_Animation(m_iAnimIndex, 0.2f, 2, 0);

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(CGameInstance::GetInstance()->Get_RandomSoundKey(L"Deidara_LargeAttack"), g_fVolume * 0.4f, m_pBoss->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Deidara_Attack_cmb02::Tick_State(_float fTimeDelta)
{
	CModel* pBossModel = m_pBoss->Get_ModelCom();
	if (pBossModel->Get_CurrAnim() != m_iAnimIndex)
		return;


	_uint iFrame = pBossModel->Get_Anim_Frame(m_iAnimIndex);


	if (iFrame >= 5 && iFrame < 10)
	{
		Follow_TargetPos(fTimeDelta);
	}

	m_pBoss->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Deidara_Attack_cmb02::Exit_State()
{

}

void CState_Deidara_Attack_cmb02::Follow_TargetPos(_float fTimeDelta)
{
	_float fMoveSpeed = m_pBoss->Get_AttackMoveSpeed();

	if (fMoveSpeed <= 0.0f)
		return;

	CTransform* pTransform = m_pBoss->Get_TransformCom();

	Vec3 vBossPos = pTransform->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vTargetPos = m_pBoss->Get_TargetPos();


	Vec3 vUp = pTransform->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();
	Vec3 vDir = vTargetPos - vBossPos;



	_float fLength = vDir.Dot(vUp);

	vTargetPos = vTargetPos - (vUp * fLength);
	vDir = vTargetPos - vBossPos;

	if (vDir.Length() < 0.001f)
		return;

	Vec3 vMove = vDir;
	vMove.Normalize();
	vMove *= fMoveSpeed * fTimeDelta;

	m_pBoss->Get_TransformCom()->LookAt_Lerp(vDir, 7.0f, fTimeDelta);

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


void CState_Deidara_Attack_cmb02::Free()
{
	__super::Free();
}
