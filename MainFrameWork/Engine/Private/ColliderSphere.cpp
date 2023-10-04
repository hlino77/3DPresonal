#include "Engine_Defines.h"
#include "GameObject.h"
#include "Transform.h"
#include "ColliderSphere.h"
#include "DebugDraw.h"
#include "Model.h"

CSphereCollider::CSphereCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:Super(pDevice, pContext, ColliderType::Sphere)
{
}

CSphereCollider::CSphereCollider(const CSphereCollider& rhs)
	:Super(rhs)
	, m_tBoundingSphere(rhs.m_tBoundingSphere)
{

}

HRESULT CSphereCollider::Initialize_Prototype()
{
	Super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSphereCollider::Initialize(void* pArg)
{
	//m_pRigidBody = m_pGameObject->GetRigidBody();

	m_pOwnerTransform = dynamic_cast<CTransform*>(m_pGameObject->Get_Component(L"Com_Transform"));
	m_pOwnerModel = dynamic_cast<CModel*>(m_pGameObject->Get_Component(L"Com_Model"));
	m_tBoundingSphere.Center = m_pOwnerTransform->Get_State(CTransform::STATE::STATE_POSITION);
	m_tBoundingSphere.Radius = 1.0f;

	if (pArg == nullptr)
		return E_FAIL;

	ColliderInfo* tCollider = static_cast<ColliderInfo*>(pArg);

	m_pOwner = tCollider->pOwner;
	m_iColLayer = tCollider->m_iLayer;
	m_bActive = tCollider->m_bActive;

	return S_OK;
}

void CSphereCollider::Tick(const _float& fTimeDelta)
{
	m_tBoundingSphere.Center = m_pOwnerTransform->Get_State(CTransform::STATE::STATE_POSITION);
	//m_tBoundingSphere.Radius = m_pOwnerTransform->Get_Scale().Length() / 2.f;

	//Vec3 scale = GetGameObject()->GetTransform()->GetLocalScale();
	//m_tBoundingSphere.Radius = m_fRadius * max(max(scale.x, scale.y), scale.z);
}

void CSphereCollider::LateTick(const _float& fTimeDelta)
{
}

void CSphereCollider::DebugRender()
{
#ifdef _DEBUG
	Super::DebugRender();

	m_pBatch->Begin();

	DX::Draw(m_pBatch, m_tBoundingSphere, Colors::Green);

	m_pBatch->End();
#endif // DEBUG
}
_bool CSphereCollider::Intersects(SimpleMath::Ray& ray, OUT _float& distance)
{
	return m_tBoundingSphere.Intersects(ray.position, ray.direction, OUT distance);
}

_bool CSphereCollider::Intersects(Super* other)
{
	ColliderType type = other->GetColliderType();

	switch (type)
	{
	case ColliderType::Sphere:
		return m_tBoundingSphere.Intersects(static_cast<CSphereCollider*>(other)->GetBoundingSphere());
	//case ColliderType::AABB:
	//	return m_tBoundingSphere.Intersects(static_cast<CAABBCollider*>(other)->GetBoundingBox());
	//case ColliderType::OBB:
	//	return m_tBoundingSphere.Intersects(static_cast<COBBCollider*>(other)->GetBoundingBox());
	//case ColliderType::Cylinder:
	//	return m_tBoundingSphere.Intersects(static_cast<CCylinderCollider*>(other)->GetBoundingCylinder());
	}

	return false;
}

void CSphereCollider::Set_Center_ToBone()
{
	Matrix matOwnerWolrd = m_pOwnerTransform->Get_WorldMatrix();

	Matrix matBone = m_pOwnerModel->Get_CurrBoneMatrix(m_iBoneIndex);

	Matrix matResult = matBone * matOwnerWolrd;

	m_tBoundingSphere.Center = Vec3(matResult.m[3]);
}


CSphereCollider* CSphereCollider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSphereCollider* pInstance = new CSphereCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSphereCollider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CSphereCollider::Clone(CGameObject* pGameObject, void* pArg)
{
	CSphereCollider* pInstance = new CSphereCollider(*this);
	pInstance->m_pGameObject = pGameObject;
	//pInstance->m_pRigidBody = pInstance->m_pGameObject->GetRigidBody();

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSphereCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSphereCollider::Free()
{
	Super::Free();
}
