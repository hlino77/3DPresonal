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

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->DebugRender();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->DebugRender();

	return S_OK;
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
