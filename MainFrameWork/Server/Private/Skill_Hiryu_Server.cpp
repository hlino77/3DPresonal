#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_Hiryu_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "RigidBody.h"


CSkill_Hiryu_Server::CSkill_Hiryu_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill_Server(pDevice, pContext)
{
}

CSkill_Hiryu_Server::CSkill_Hiryu_Server(const CSkill_Hiryu_Server& rhs)
	: CSkill_Server(rhs)
{
}

HRESULT CSkill_Hiryu_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Hiryu_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_pRigidBody->SetCompareGruond(false);
	m_pRigidBody->UseGravity(false);


	Set_Active(false);
    return S_OK;
}

void CSkill_Hiryu_Server::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fCurrTime += fTimeDelta;

	if (m_bExplosion)
	{
		if (m_fCurrTime >= m_fExplosionTime)
		{
			Set_Die();
			Set_Active(false);
		}
	}
	else
	{
		m_fCurrTime += fTimeDelta;

		if (m_fCurrTime >= m_fTimer)
		{
			Explosion();
		}
	}




	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vTargetPos = m_pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vTargetPos.y += 0.5f;

	Vec3 vDir = vTargetPos - vPos;
	vDir.Normalize();


	m_pTransformCom->SetUp_Lerp(vDir, 3.0f, fTimeDelta);
	m_pTransformCom->LookAt_Lerp_ForLand(vDir, 3.0f, fTimeDelta);
	m_pTransformCom->Go_Straight(8.0f, fTimeDelta);

	if (m_fCurrScale < 0.01f)
	{
		m_fCurrScale += 0.01f * fTimeDelta;

		if (m_fCurrScale > 0.01f)
			m_fCurrScale = 0.01f;

		m_pTransformCom->Set_Scale(Vec3(m_fCurrScale, m_fCurrScale, m_fCurrScale));
	}
}

void CSkill_Hiryu_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	/*m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_SkillInfo();
	}*/
}

HRESULT CSkill_Hiryu_Server::Render()
{
	return S_OK;
}

void CSkill_Hiryu_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			Explosion();
			Send_Collision(iColLayer, pOther, true);
		}
	}
}

void CSkill_Hiryu_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_Hiryu_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
}


HRESULT CSkill_Hiryu_Server::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(1.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.2f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(4.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.2f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(1, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	return S_OK;
}

void CSkill_Hiryu_Server::Send_SkillInfo()
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

void CSkill_Hiryu_Server::Shoot_Hiryu(CGameObject* pTargetObject, Vec3 vPos, Vec3 vLook)
{
	m_bActive = true;

	m_pNearTarget = pTargetObject;
	Send_NearTarget();


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt_ForLandObject(vPos + vLook);

	m_fTimer = 5.0f;
	m_fCurrTime = 0.0f;
	m_bExplosion = false;


	m_fCurrScale = 0.001f;
	m_pTransformCom->Set_Scale(Vec3(m_fCurrScale, m_fCurrScale, m_fCurrScale));


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
}

void CSkill_Hiryu_Server::Explosion()
{
	m_bRender = false;
	m_bExplosion = true;
	
	m_fCurrTime = 0.0f;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);
}



HRESULT CSkill_Hiryu_Server::Ready_Components()
{
	__super::Ready_Components();

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	/////* For.Com_Model */
	//wstring strComName = L"Prototype_Component_Model_Hiryu";
	//if (FAILED(__super::Add_Component(pGameInstance->Get_CurrLevelIndex(), strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}



CSkill_Hiryu_Server* CSkill_Hiryu_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_Hiryu_Server* pInstance = new CSkill_Hiryu_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_Hiryu_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_Hiryu_Server::Clone(void* pArg)
{
	CSkill_Hiryu_Server* pInstance = new CSkill_Hiryu_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_Hiryu_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_Hiryu_Server::Free()
{
	__super::Free();
}
