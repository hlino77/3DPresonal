#include "stdafx.h"
#include "State_Naruto_Skill_Rasengun_RunStart.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Skill_Rasengun.h"


CState_Naruto_Skill_Rasengun_RunStart::CState_Naruto_Skill_Rasengun_RunStart(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Skill_Rasengun_RunStart::CState_Naruto_Skill_Rasengun_RunStart(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Skill_Rasengun_RunStart::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Rasengun_Charge_Lv2toLv3_ConectToRun", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Skill_Rasengun_RunStart::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Skill_Rasengun_RunStart::Tick_State_NoneControl;


	return S_OK;
}

void CState_Naruto_Skill_Rasengun_RunStart::Enter_State()
{
	Set_NearTarget();

	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);

	m_pPlayer->Set_EnemyBodyHit(false);

	m_pPlayer->Get_Rasengun()->Set_Run();

	CGameInstance::GetInstance()->PlaySound_Distance_LoopChannel(L"Naruto_Rasengan.wav", g_fVolume * 0.4f, m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION), 40.0f);
}

void CState_Naruto_Skill_Rasengun_RunStart::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_RunStart::Exit_State()
{

}

void CState_Naruto_Skill_Rasengun_RunStart::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();


	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
	{
		if (m_pPlayer->Is_EnemyBodyHit())
		{
			m_pPlayer->Set_State(L"Rasengun_Attack");
			return;
		}
		else
		{
			m_pPlayer->Set_State(L"Rasengun_RunLoop");
			return;
		}
	}


	Follow_Target(fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_RunStart::Tick_State_NoneControl(_float fTimeDelta)
{
	Follow_Target(fTimeDelta);

	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Skill_Rasengun_RunStart::Free()
{
	__super::Free();
}

void CState_Naruto_Skill_Rasengun_RunStart::Set_NearTarget()
{
	m_pPlayer->Find_NearTarget();


	CGameObject* pNearTarget = m_pPlayer->Get_NearTarget();

	if (pNearTarget)
	{
		Vec3 vTargetPos = pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vPos = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		_float fDistance = (vTargetPos - vPos).Length();

		if (fDistance > 15.0f)
		{
			m_pPlayer->Set_NearTarget(nullptr);
		}
	}

	m_pPlayer->Send_NearTarget();


	Vec3 vLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();

	m_pPlayer->Set_TargetPos(vLook);
}

void CState_Naruto_Skill_Rasengun_RunStart::Follow_Target(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	CGameObject* pTarget = m_pPlayer->Get_NearTarget();


	if (pTarget == nullptr)
	{
		Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		pTransform->LookAt_Lerp(vTargetPos, 10.0f, fTimeDelta);
		pTransform->Go_Straight(15.0f, fTimeDelta);
		return;
	}

	Vec3 vTargetPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);

	if (fDistance > 1.4f)
		pTransform->Go_Straight(15.0f, fTimeDelta);
}
