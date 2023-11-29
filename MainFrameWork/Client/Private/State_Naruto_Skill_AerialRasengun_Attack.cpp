#include "stdafx.h"
#include "State_Naruto_Skill_AerialRasengun_Attack.h"
#include "Player_Naruto.h"
#include "Model.h"
#include "StateMachine.h"
#include "Key_Manager.h"
#include "GameInstance.h"
#include "Camera_Player.h"
#include "Skill_Rasengun.h"
#include "RigidBody.h"


CState_Naruto_Skill_AerialRasengun_Attack::CState_Naruto_Skill_AerialRasengun_Attack(const wstring& strStateName, CPlayer_Naruto* pPlayer)
	:CState(strStateName)
	, m_pPlayer(pPlayer)
{
}

CState_Naruto_Skill_AerialRasengun_Attack::CState_Naruto_Skill_AerialRasengun_Attack(const CState& rhs, CStateMachine* pMachine)
	:CState(rhs, pMachine)
{
}

HRESULT CState_Naruto_Skill_AerialRasengun_Attack::Initialize()
{
	m_iAnimIndex = m_pPlayer->Get_ModelCom()->Initailize_FindAnimation(L"Ninjutsu_Aerial_Rasengan_End", 1.5f);

	if (m_iAnimIndex == -1)
		return E_FAIL;

	if(m_pPlayer->Is_Control())
		m_TickFunc = &CState_Naruto_Skill_AerialRasengun_Attack::Tick_State_Control;
	else
		m_TickFunc = &CState_Naruto_Skill_AerialRasengun_Attack::Tick_State_NoneControl;


	return S_OK;
}

void CState_Naruto_Skill_AerialRasengun_Attack::Enter_State()
{
	m_pPlayer->Reserve_Animation(m_iAnimIndex, 0.1f, 0, 0);
	m_bAttack = false;
}

void CState_Naruto_Skill_AerialRasengun_Attack::Tick_State(_float fTimeDelta)
{
	m_TickFunc(*this, fTimeDelta);
}

void CState_Naruto_Skill_AerialRasengun_Attack::Exit_State()
{
	m_pPlayer->Set_Invincible(false);


	m_pPlayer->Get_RigidBody()->UseDrag(true);
	m_pPlayer->Set_Gravity(true);
	m_pPlayer->Get_RigidBody()->SetCompareGruond(true);
	m_pPlayer->Set_DefaultUp(true);
}

void CState_Naruto_Skill_AerialRasengun_Attack::Tick_State_Control(_float fTimeDelta)
{
	CModel* pPlayerModel = m_pPlayer->Get_ModelCom();

	_uint iFrame = pPlayerModel->Get_Anim_Frame(m_iAnimIndex);

	if (m_bAttack == false)
		LookAt_Target(fTimeDelta);


	if (m_bAttack == false && iFrame > 5)
	{
		m_pPlayer->Get_Rasengun()->Explosion();
		m_bAttack = true;
	}


	if (pPlayerModel->Is_AnimationEnd(m_iAnimIndex))
	{
		m_pPlayer->Set_State(L"Fall_Front");
	}


	//Prototype_GameObject_RasenganCircle
}

void CState_Naruto_Skill_AerialRasengun_Attack::Tick_State_NoneControl(_float fTimeDelta)
{
	m_pPlayer->Follow_ServerPos(0.01f, 6.0f * fTimeDelta);
}

void CState_Naruto_Skill_AerialRasengun_Attack::LookAt_Target(_float fTimeDelta)
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
	vTargetPos.y += 0.5f;
	Vec3 vPos = pTransform->Get_State(CTransform::STATE_POSITION);
	Vec3 vDir = vTargetPos - vPos;

	_float fDistance = vDir.Length();
	vDir.Normalize();

	pTransform->LookAt(vTargetPos);
}

void CState_Naruto_Skill_AerialRasengun_Attack::Free()
{
	__super::Free();
}
