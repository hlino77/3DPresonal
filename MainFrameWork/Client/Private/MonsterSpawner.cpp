#include "stdafx.h"
#include "GameInstance.h"
#include "MonsterSpawner.h"
#include "AsUtils.h"
#include "CollisionManager.h"
#include "RigidBody.h"
#include "NavigationMgr.h"
#include "ColliderSphere.h"



CMonsterSpawner::CMonsterSpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"MonsterSpawner", OBJ_TYPE::SPAWNER)
{
}

CMonsterSpawner::CMonsterSpawner(const CMonsterSpawner& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterSpawner::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonsterSpawner::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

    return S_OK;
}

void CMonsterSpawner::Tick(_float fTimeDelta)
{

}

void CMonsterSpawner::LateTick(_float fTimeDelta)
{
}

HRESULT CMonsterSpawner::Render()
{

	return S_OK;
}

void CMonsterSpawner::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{

}

void CMonsterSpawner::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CMonsterSpawner::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	
}





HRESULT CMonsterSpawner::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	Safe_Release(pGameInstance);


    return S_OK;
}



void CMonsterSpawner::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_bDie = true;
}





void CMonsterSpawner::Free()
{
	__super::Free();



	Safe_Release(m_pModelCom);

	Safe_Release(m_pTransformCom);
}

CMonsterSpawner* CMonsterSpawner::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterSpawner* pInstance = new CMonsterSpawner(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CMonsterSpawner" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterSpawner::Clone(void* pArg)
{
	CMonsterSpawner* pInstance = new CMonsterSpawner(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CMonsterSpawner" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}
