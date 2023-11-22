#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_Meteor_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "RigidBody.h"


CSkill_Meteor_Server::CSkill_Meteor_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill_Server(pDevice, pContext)
{
}

CSkill_Meteor_Server::CSkill_Meteor_Server(const CSkill_Meteor_Server& rhs)
	: CSkill_Server(rhs)
{
}

HRESULT CSkill_Meteor_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Meteor_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);




	Set_Active(false);


	m_fExplosionTime = 0.1f;

	m_vStartPos = Vec3(78.95f, 98.0f, -52.07f);

    return S_OK;
}

void CSkill_Meteor_Server::Tick(_float fTimeDelta)
{
	if (m_bExplosion)
	{
		m_fCurrTime += fTimeDelta;
		if (m_fCurrTime >= m_fExplosionTime)
		{
			Set_Die();
			Set_Active(false);
		}
	}
	else
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos.y -= 10.0f * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


		if (vPos.y <= 34.0f)
		{
			Send_Explosion();
			Explosion();
		}
	}
}

void CSkill_Meteor_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	/*m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_SkillInfo();
	}*/
}

HRESULT CSkill_Meteor_Server::Render()
{
	return S_OK;
}

void CSkill_Meteor_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_Meteor_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_Meteor_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
}


HRESULT CSkill_Meteor_Server::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(1.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, -0.2f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(16.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, -0.2f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(1, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	return S_OK;
}

void CSkill_Meteor_Server::Send_SkillInfo()
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

void CSkill_Meteor_Server::Explosion()
{
	m_bRender = false;
	m_bExplosion = true;
	
	m_fCurrTime = 0.0f;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);
}

void CSkill_Meteor_Server::Appear()
{
	Set_Active(true);

	m_bExplosion = false;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vStartPos);
}



HRESULT CSkill_Meteor_Server::Ready_Components()
{
	__super::Ready_Components();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	Safe_Release(pGameInstance);

	return S_OK;
}



CSkill_Meteor_Server* CSkill_Meteor_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Meteor_Server* pInstance = new CSkill_Meteor_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_Meteor_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Meteor_Server::Clone(void* pArg)
{
	CSkill_Meteor_Server* pInstance = new CSkill_Meteor_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_Meteor_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Meteor_Server::Free()
{
	__super::Free();
}
