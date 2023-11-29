#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Boss_Deidara.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "ServerSessionManager.h"
#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "State_Deidara_Idle.h"
#include "State_Deidara_Appear.h"
#include "State_Deidara_HitMiddle.h"
#include "Monster_C2Dragon.h"
#include "State_Deidara_Skill_C2Dragon.h"
#include "State_Deidara_Die.h"
#include "State_Deidara_DownToFloor.h"
#include "State_Deidara_Dying_Normal.h"
#include "State_Deidara_FallBehind.h"
#include "State_Deidara_GetUp.h"
#include "State_Deidara_HitSpinBlowDown.h"
#include "State_Deidara_HitSpinBlowUp.h"
#include "State_Deidara_ChasePlayer.h"
#include "State_Deidara_Attack_cmb01.h"
#include "State_Deidara_Attack_cmb02.h"
#include "Skill_TwinBird.h"
#include "State_Deidara_Skill_TwinBirds.h"
#include "LineCircle.h"
#include "Pool.h"
#include "Player.h"


CBoss_Deidara::CBoss_Deidara(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CBoss(pDevice, pContext)
{
}

CBoss_Deidara::CBoss_Deidara(const CBoss_Deidara& rhs)
	: CBoss(rhs)
{
}

HRESULT CBoss_Deidara::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBoss_Deidara::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();


	m_fAttackMoveSpeed = 8.0f;

	m_pTwinBird[0] = nullptr;
	m_pTwinBird[1] = nullptr;


	m_iHp = 100;
	m_iMaxHp = 100;


	if (FAILED(Ready_HP_UI(0)))
		return E_FAIL;


	return S_OK;
}

void CBoss_Deidara::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CBoss_Deidara::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	Set_Colliders();
}

HRESULT CBoss_Deidara::Render()
{
	__super::Render();

	return S_OK;
}

void CBoss_Deidara::Set_Skill(CGameObject* pGameObject)
{
	WRITE_LOCK
	if (m_pTwinBird[0] == nullptr)
		m_pTwinBird[0] = dynamic_cast<CSkill_TwinBird*>(pGameObject);
	else
		m_pTwinBird[1] = dynamic_cast<CSkill_TwinBird*>(pGameObject);
}

void CBoss_Deidara::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			if (m_bHitEffect == false)
			{
				if (dynamic_cast<CPlayer*>(pOther->Get_Owner())->Is_Invincible() == false)
					Effect_Hit();
				m_bHitEffect = true;
			}
		}

		return;
	}
}

void CBoss_Deidara::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CBoss_Deidara::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
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

void CBoss_Deidara::Set_Colliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();

}

void CBoss_Deidara::Spawn_C2Dragon()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();


	vPos += vLook * -1.5f;
	m_pC2Dragon->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
	m_pC2Dragon->Get_TransformCom()->LookAt_ForLandObject(vPos + vLook);

	m_pC2Dragon->Spawn();
}

void CBoss_Deidara::Shoot_TwinBirds()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	Vec3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);


	vRight.Normalize();
	vLook.Normalize();
	vUp.Normalize();

	{
		Vec3 vTargetPos = vPos + (vLook * 0.5f) + (vRight * 0.5f) + (vUp * 2.5f);
		Vec3 vTargetLook = XMVector3Rotate(vLook, Quaternion::CreateFromAxisAngle(vUp, XMConvertToRadians(30.0f)));

		m_pTwinBird[0]->Shoot_TwinBird(m_pNearTarget, vTargetPos, vTargetLook);
	}

	{
		Vec3 vTargetPos = vPos + (vLook * 0.5f) + (vRight * -0.5f) + (vUp * 1.5f);
		Vec3 vTargetLook = XMVector3Rotate(vLook, Quaternion::CreateFromAxisAngle(vUp, XMConvertToRadians(-30.0f)));

		m_pTwinBird[1]->Shoot_TwinBird(m_pNearTarget, vTargetPos, vTargetLook);
	}
}

void CBoss_Deidara::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_pC2Dragon->Set_Die();
	m_pTwinBird[0]->Set_Die();
	m_pTwinBird[1]->Set_Die();

	m_bDie = true;
}

void CBoss_Deidara::Effect_Hit()
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


	Vec4 vColor(1.0f, 1.0f, 0.0f, 1.0f);
	Vec4 vBlurColor(1.0f, 0.3f, 0.0f, 1.0f);

	for (_uint i = 0; i < 50; ++i)
	{
		CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
		if (pLineCircle)
		{
			pLineCircle->Appear(vPos, vColor, vBlurColor, 1.0f);
		}
	}
}



HRESULT CBoss_Deidara::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CBoss_Deidara::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Deidara_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Appear", new CState_Deidara_Appear(L"Appear", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_Deidara_HitMiddle(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"Skill_C2Dragon", new CState_Deidara_Skill_C2Dragon(L"Skill_C2Dragon", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_Deidara_HitSpinBlowUp(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowDown", new CState_Deidara_HitSpinBlowDown(L"Hit_SpinBlowDown", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_Deidara_GetUp(L"GetUp", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_Deidara_DownToFloor(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"Die", new CState_Deidara_Die(L"Die", this));
	m_pStateMachine->Add_State(L"Dying_Normal", new CState_Deidara_Dying_Normal(L"Dying_Normal", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_Deidara_FallBehind(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"ChaseTarget", new CState_Deidara_ChasePlayer(L"ChaseTarget", this));
	m_pStateMachine->Add_State(L"Attack_cmb01", new CState_Deidara_Attack_cmb01(L"Attack_cmb01", this));
	m_pStateMachine->Add_State(L"Attack_cmb02", new CState_Deidara_Attack_cmb02(L"Attack_cmb02", this));
	m_pStateMachine->Add_State(L"Skill_TwinBirds", new CState_Deidara_Skill_TwinBirds(L"Skill_TwinBirds", this));



	m_pStateMachine->Change_State(L"Appear");

	return S_OK;
}


CBoss_Deidara* CBoss_Deidara::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Deidara* pInstance = new CBoss_Deidara(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CBoss_Deidara");
		Safe_Release(pInstance);
	}

	return pInstance;	
}

CGameObject* CBoss_Deidara::Clone(void* pArg)
{
	CBoss_Deidara* pInstance = new CBoss_Deidara(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CBoss_Deidara");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Deidara::Free()
{
	__super::Free();
}
