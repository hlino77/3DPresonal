#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Boss_Madara.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "ServerSessionManager.h"
#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "State_Madara_Appear.h"
#include "State_Madara_Idle.h"
#include "State_Madara_DownToFloor.h"
#include "State_Madara_FallBehind.h"
#include "State_Madara_GetUp.h"
#include "State_Madara_HitMiddle.h"
#include "State_Madara_HitSpinBlowDown.h"
#include "State_Madara_HitSpinBlowUp.h"
#include "NavigationMgr.h"
#include "State_Madara_TeleportAttack.h"
#include "State_Madara_Attack_Punch.h"
#include "State_Madara_Attack_TurnKick.h"
#include "State_Madara_Attack_DoubleTurnKick.h"
#include "LineCircle.h"
#include "Pool.h"
#include "State_Madara_Skill_Meteor.h"
#include "Skill_Meteor.h"
#include "Player.h"
#include "Skill_MadaraFireBall.h"
#include "State_Madara_Skill_FireBall_End.h"
#include "State_Madara_Skill_FireBall_Loop.h"
#include "State_Madara_Skill_FireBall_Start.h"
#include "Teleport.h"
#include "State_Madara_Die.h"
#include "State_Madara_Dying_Normal.h"


CBoss_Madara::CBoss_Madara(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBoss(pDevice, pContext)
{
}

CBoss_Madara::CBoss_Madara(const CBoss_Madara& rhs)
	: CBoss(rhs)
{
}

HRESULT CBoss_Madara::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Madara::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();


	m_fAttackMoveSpeed = 12.0f;



	m_iHp = 200;
	m_iMaxHp = 200;

	if (FAILED(Ready_HP_UI(1)))
		return E_FAIL;


	CNavigationMgr::GetInstance()->Reset_Navigation();
	CNavigationMgr::GetInstance()->Add_Navigation(L"Madara.navi");

	return S_OK;
}

void CBoss_Madara::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CBoss_Madara::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	Set_Colliders();
}

HRESULT CBoss_Madara::Render()
{
	__super::Render();

	return S_OK;
}

void CBoss_Madara::Set_Skill(CGameObject* pGameObject)
{
	WRITE_LOCK
	if (pGameObject->Get_ModelName() == L"Meteor")
		m_pMeteor = dynamic_cast<CSkill_Meteor*>(pGameObject);
	if (pGameObject->Get_ModelName() == L"MadaraFireBall")
		m_pFireBall = dynamic_cast<CSkill_MadaraFireBall*>(pGameObject);

}

void CBoss_Madara::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			if (m_bHitEffect == false)
			{
				if(dynamic_cast<CPlayer*>(pOther->Get_Owner())->Is_Invincible() == false)
					Effect_Hit();
				m_bHitEffect = true;
			}
		}

		return;
	}
}

void CBoss_Madara::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CBoss_Madara::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			m_bHitEffect = false;
		}
		return;
	}
}

void CBoss_Madara::Set_Colliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();

}


void CBoss_Madara::Set_Die()
{
}

void CBoss_Madara::Effect_Teleport()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();

	vPos += vUp * 0.5f;

	CTeleport* pTeleport = CPool<CTeleport>::Get_Obj();
	pTeleport->Appear(vPos, vUp);
}

HRESULT CBoss_Madara::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CBoss_Madara::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Madara_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Appear", new CState_Madara_Appear(L"Appear", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_Madara_HitMiddle(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_Madara_HitSpinBlowUp(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowDown", new CState_Madara_HitSpinBlowDown(L"Hit_SpinBlowDown", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_Madara_GetUp(L"GetUp", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_Madara_DownToFloor(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_Madara_FallBehind(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"TeleportAttack", new CState_Madara_TeleportAttack(L"TeleportAttack", this));
	m_pStateMachine->Add_State(L"Attack_Punch", new CState_Madara_Attack_Punch(L"Attack_Punch", this));
	m_pStateMachine->Add_State(L"Attack_TurnKick", new CState_Madara_Attack_TurnKick(L"Attack_TurnKick", this));
	m_pStateMachine->Add_State(L"Attack_DoubleTurnKick", new CState_Madara_Attack_DoubleTurnKick(L"Attack_DoubleTurnKick", this));
	m_pStateMachine->Add_State(L"Skill_Meteor", new CState_Madara_Skill_Meteor(L"Skill_Meteor", this));

	m_pStateMachine->Add_State(L"Skill_FireBall_Start", new CState_Madara_Skill_FireBall_Start(L"Skill_FireBall_Start", this));
	m_pStateMachine->Add_State(L"Skill_FireBall_Loop", new CState_Madara_Skill_FireBall_Loop(L"Skill_FireBall_Loop", this));
	m_pStateMachine->Add_State(L"Skill_FireBall_End", new CState_Madara_Skill_FireBall_End(L"Skill_FireBall_End", this));

	m_pStateMachine->Add_State(L"Die", new CState_Madara_Die(L"Die", this));
	m_pStateMachine->Add_State(L"Dying_Normal", new CState_Madara_Dying_Normal(L"Dying_Normal", this));

	m_pStateMachine->Change_State(L"Appear");

	return S_OK;
}

void CBoss_Madara::Effect_Hit()
{
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();
	vLook.Normalize();
	vPos += vUp * 0.7f;
	vPos += vLook * 1.0f;


	/*Vec3 vColor(0.19f, 0.2f, 0.29f);
	vColor *= 0.5f;*/

	
	Vec4 vColor(0.0f, 0.0f, 0.0f, 1.0f);
	Vec4 vBlurColor(1.0f, 0.0f, 0.0f, 1.0f);

	for (_uint i = 0; i < 50; ++i)
	{
		CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
		if (pLineCircle)
		{
			pLineCircle->Appear(vPos, vColor, vBlurColor, 1.0f);
		}
	}
}


CBoss_Madara* CBoss_Madara::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Madara* pInstance = new CBoss_Madara(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBoss_Madara");
		Safe_Release(pInstance);
	}

	return pInstance;	
}

CGameObject* CBoss_Madara::Clone(void* pArg)
{
	CBoss_Madara* pInstance = new CBoss_Madara(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBoss_Madara");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Madara::Free()
{
	__super::Free();
}
