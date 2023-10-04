#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Monster_WhiteZetsu.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "ServerSessionManager.h"
#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "State_WhiteZetsu_Idle.h"

CMonster_WhiteZetsu::CMonster_WhiteZetsu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CMonster(pDevice, pContext)
{
}

CMonster_WhiteZetsu::CMonster_WhiteZetsu(const CMonster_WhiteZetsu& rhs)
	: CMonster(rhs)
{
}

HRESULT CMonster_WhiteZetsu::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonster_WhiteZetsu::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();


	return S_OK;
}

void CMonster_WhiteZetsu::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMonster_WhiteZetsu::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	Set_Colliders();
}

HRESULT CMonster_WhiteZetsu::Render()
{
	__super::Render();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->DebugRender();
	return S_OK;
}

void CMonster_WhiteZetsu::Set_Colliders()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE::STATE_UP);
	vUp.Normalize();


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center(vPos + vUp * 0.7f);
}



HRESULT CMonster_WhiteZetsu::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CMonster_WhiteZetsu::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle_Loop", new CState_WhiteZetsu_Idle(L"Idle_Loop", this));
	return S_OK;
}


CMonster_WhiteZetsu* CMonster_WhiteZetsu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_WhiteZetsu* pInstance = new CMonster_WhiteZetsu(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CMonster_WhiteZetsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_WhiteZetsu::Clone(void* pArg)
{
	CMonster_WhiteZetsu* pInstance = new CMonster_WhiteZetsu(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMonster_WhiteZetsu");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_WhiteZetsu::Free()
{
	__super::Free();
}
