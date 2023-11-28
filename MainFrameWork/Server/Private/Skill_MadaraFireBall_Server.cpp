#include "stdafx.h"
#include "GameInstance.h"
#include "Skill_MadaraFireBall_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "RigidBody.h"


CSkill_MadaraFireBall_Server::CSkill_MadaraFireBall_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSkill_Server(pDevice, pContext)
{
}

CSkill_MadaraFireBall_Server::CSkill_MadaraFireBall_Server(const CSkill_MadaraFireBall_Server& rhs)
	: CSkill_Server(rhs)
{
}

HRESULT CSkill_MadaraFireBall_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_MadaraFireBall_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_fExplosionTime = 0.1f;
	m_fSkillTime = 2.0f;

	Set_Active(false);
    return S_OK;
}

void CSkill_MadaraFireBall_Server::Tick(_float fTimeDelta)
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
		m_fCurrTime += fTimeDelta;

		if (m_fCurrTime >= m_fSkillTime)
		{
			Explosion();
		}

		Follow_Target(fTimeDelta);
	}
}

void CSkill_MadaraFireBall_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	/*m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_SkillInfo();
	}*/
}

HRESULT CSkill_MadaraFireBall_Server::Render()
{
	return S_OK;
}

void CSkill_MadaraFireBall_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			if(m_bExplosion == false)
				Explosion();
		}
	}
}

void CSkill_MadaraFireBall_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	
}

void CSkill_MadaraFireBall_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
}


HRESULT CSkill_MadaraFireBall_Server::Ready_Coliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(4.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);


	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(5.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.0f, 0.0f));
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_AttackCollider(1, (_uint)COLLIDER_ATTACK::SPINBLOWUP, false);
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);

	return S_OK;
}

void CSkill_MadaraFireBall_Server::Send_SkillInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tObject = pkt.add_tobject();

	tObject->set_iobjectid(m_iObjectID);
	tObject->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_SKILL);


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

void CSkill_MadaraFireBall_Server::Appear()
{
	Set_Active(true);
	m_bExplosion = false;
	m_fCurrTime = 0.0f;

	m_pNearTarget = m_pSkillOwner->Get_NearTarget();

	Vec3 vPlayerPos = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vPlayerLook = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	Vec3 vPlayerUp = m_pSkillOwner->Get_TransformCom()->Get_State(CTransform::STATE_UP);
	vPlayerLook.Normalize();
	vPlayerUp.Normalize();

	Vec3 vPos = vPlayerPos + (vPlayerLook * 4.0f) + (vPlayerUp * 3.5f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->LookAt(vPos + vPlayerLook);
	Send_SkillInfo();

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
}

void CSkill_MadaraFireBall_Server::Follow_Target(_float fTimeDelta)
{
	if (m_pNearTarget)
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		Vec3 vTargetPos = m_pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vTargetPos.y += 1.5f;

		Vec3 vDir = vTargetPos - vPos;
		vDir.Normalize();

		m_pTransformCom->SetUp_Lerp(vDir, 0.5f, fTimeDelta);
		m_pTransformCom->LookAt_Lerp_ForLand(vDir, 0.5f, fTimeDelta);
	}

	m_pTransformCom->Go_Straight(10.0f, fTimeDelta);
}

void CSkill_MadaraFireBall_Server::Explosion()
{
	m_bRender = false;
	m_bExplosion = true;
	
	m_fCurrTime = 0.0f;

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(true);

	Send_Explosion();
}



HRESULT CSkill_MadaraFireBall_Server::Ready_Components()
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



CSkill_MadaraFireBall_Server* CSkill_MadaraFireBall_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkill_MadaraFireBall_Server* pInstance = new CSkill_MadaraFireBall_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CSkill_MadaraFireBall_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSkill_MadaraFireBall_Server::Clone(void* pArg)
{
	CSkill_MadaraFireBall_Server* pInstance = new CSkill_MadaraFireBall_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CSkill_MadaraFireBall_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkill_MadaraFireBall_Server::Free()
{
	__super::Free();
}
