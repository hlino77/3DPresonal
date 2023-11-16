#include "stdafx.h"
#include "State_Sasuke_Skill_Chidori_Attack.h"
#include "Player_Sasuke.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Skill_Chidori.h"



CState_Sasuke_Skill_Chidori_Attack::CState_Sasuke_Skill_Chidori_Attack(const wstring& strStateName, CPlayer_Sasuke* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Sasuke_Skill_Chidori_Attack::CState_Sasuke_Skill_Chidori_Attack(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Sasuke_Skill_Chidori_Attack::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Chidori_Attack_End", 1.2f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Sasuke_Skill_Chidori_Attack::Tick_State_Control;
	else
		m_TickFunc = &CState_Sasuke_Skill_Chidori_Attack::Tick_State_NoneControl;


	return S_OK;
}

void CState_Sasuke_Skill_Chidori_Attack::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bAttack = false;
}

void CState_Sasuke_Skill_Chidori_Attack::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_Attack::Exit_State()
{
	m_pPlayer->Set_Invincible(false);
}

void CState_Sasuke_Skill_Chidori_Attack::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();

	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);

	if (m_bAttack == false && iFrame > 3)
	{
		m_pPlayer->Get_Chidori()->Explosion();
		m_bAttack = true;
	}


	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
	{
		m_pPlayer->Set_State(L"Idle");
	}


	//Prototype_GameObject_RasenganCircle

	
}

void CState_Sasuke_Skill_Chidori_Attack::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_Attack::LookAt_Target(_float fTimeDelta)
{
	CTransform* pTransform = m_pPlayer->Get_TransformCom();

	CGameObject* pTarget = m_pPlayer->Get_NearTarget();


	if (pTarget == nullptr)
	{
		Vec3 vTargetPos = m_pPlayer->Get_TargetPos();
		pTransform->LookAt_Lerp(vTargetPos, 10.0f, fTimeDelta);
		return;
	}

	Vec3 vTargetPos = pTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();

	pTransform->LookAt_Lerp(vDir, 10.0f, fTimeDelta);
}

void CState_Sasuke_Skill_Chidori_Attack::Free()
{
	__super::Free();
}
