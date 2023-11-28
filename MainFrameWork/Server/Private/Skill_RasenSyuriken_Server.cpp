#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_RasenSyuriken_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "RigidBody.h"
#include "CollisionManager.h"


CSkill_RasenSyuriken_Server::CSkill_RasenSyuriken_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill_Server(pDevice, pContext)
{
}

CSkill_RasenSyuriken_Server::CSkill_RasenSyuriken_Server(const CSkill_RasenSyuriken_Server& rhs)
	: CSkill_Server(rhs)
{
}

HRESULT CSkill_RasenSyuriken_Server::Initialize_Prototype()
{



    return S_OK;
}

HRESULT CSkill_RasenSyuriken_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_bExplosion = false;

	Set_Active(true);

    return S_OK;
}

void CSkill_RasenSyuriken_Server::Tick(_float fTimeDelta)
{
	//__super::Tick(fTimeDelta);

}

void CSkill_RasenSyuriken_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);


	/*m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_SkillInfo();
	}*/
}

HRESULT CSkill_RasenSyuriken_Server::Render()
{
	return S_OK;
}

void CSkill_RasenSyuriken_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS)
	{
		Send_Collision(iColLayer, pOther, true);
	}
}

void CSkill_RasenSyuriken_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_RasenSyuriken_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS)
		{
			Send_Collision(iColLayer, pOther, false);
		}
	}
}


HRESULT CSkill_RasenSyuriken_Server::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(3.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(4.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(10, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);

	return S_OK;
}

void CSkill_RasenSyuriken_Server::Send_SkillInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tObject = pkt.add_tobject();

	tObject->set_iobjectid(m_iObjectID);
	tObject->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_MONSTER);


	auto vTargetPos = tObject->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tObject->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);
}


void CSkill_RasenSyuriken_Server::Explosion()
{
	m_bExplosion = true;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();
}



HRESULT CSkill_RasenSyuriken_Server::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_UseLock_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = true;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_BODY;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_SphereColider"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;

		m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_BODY, pCollider);
		CCollisionManager::GetInstance()->Add_Colider(pCollider);
	}

	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = false;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_ATTACK;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_ColliderAttack"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;
		if (pCollider)
			m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_ATTACK, pCollider);
		CCollisionManager::GetInstance()->Add_Colider(pCollider);
	}


	Safe_Release(pGameInstance);

	return S_OK;
}



CSkill_RasenSyuriken_Server* CSkill_RasenSyuriken_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_RasenSyuriken_Server* pInstance = new CSkill_RasenSyuriken_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_RasenSyuriken_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_RasenSyuriken_Server::Clone(void* pArg)
{
	CSkill_RasenSyuriken_Server* pInstance = new CSkill_RasenSyuriken_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_RasenSyuriken_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_RasenSyuriken_Server::Free()
{
	__super::Free();
}
