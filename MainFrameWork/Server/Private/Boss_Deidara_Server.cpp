#include "stdafx.h"
#include "GameInstance.h"
#include "Boss_Deidara_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "State_Deidara_Appear_Server.h"
#include "State_Deidara_Idle_Server.h"
#include "State_Deidara_HitMiddle_Server.h"
#include "State_Deidara_Skill_C2Dragon_Server.h"
#include "Monster_C2Dragon_Server.h"

CBoss_Deidara_Server::CBoss_Deidara_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBoss_Server(pDevice, pContext)
{
}

CBoss_Deidara_Server::CBoss_Deidara_Server(const CBoss_Deidara_Server& rhs)
	: CBoss_Server(rhs)
{
}

HRESULT CBoss_Deidara_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Deidara_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();

	m_iHp = 6;

    return S_OK;
}

void CBoss_Deidara_Server::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CBoss_Deidara_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_BossInfo();
	}
}

HRESULT CBoss_Deidara_Server::Render()
{
	__super::Render();

	return S_OK;
}

void CBoss_Deidara_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		Hit_Attack(pOther);
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Add_CollisionStay(iColLayer, pOther);
		return;
	}
}

void CBoss_Deidara_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Body_Collision(pOther->Get_Owner());
	}
}

void CBoss_Deidara_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Delete_CollisionStay(iColLayer, pOther);
		return;
	}

}

void CBoss_Deidara_Server::Send_BossInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tObject = pkt.add_tobject();

	tObject->set_iobjectid(m_iObjectID);
	tObject->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_BOSS);


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


void CBoss_Deidara_Server::Update_NearTarget(_float fTimeDelta)
{
	/*m_fFindTargetTime += fTimeDelta;
	if (m_fFindTargetTime >= 1.0f)
	{
		Find_NearTarget();
		if (m_pNearTarget)
		{
			_float fDistance = Get_NearTargetDistance();
			if (fDistance <= m_fFollowDistance)
			{
				if(m_pStateMachine->Get_CurrState() != L"ChaseTarget" && fDistance > 1.0f)
					Set_State(L"ChaseTarget");

				Send_NearTarget();
			}
		}
		m_fFindTargetTime = 0.0f;
	}*/

}

void CBoss_Deidara_Server::Attack_Random()
{
	/*_uint iAttack = rand() % 2;

	switch (iAttack)
	{
	case 0:
		Set_State(L"Attack_Normal");
		break;
	case 1:
		Set_State(L"Attack_Kick");
		break;
	}*/
}

void CBoss_Deidara_Server::Spawn_C2Dragon()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();


	vPos += vLook * -1.5f;
	m_pC2Dragon->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
	m_pC2Dragon->Get_TransformCom()->LookAt_ForLandObject(vPos + vLook);

	m_pC2Dragon->Spawn();
}

HRESULT CBoss_Deidara_Server::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

void CBoss_Deidara_Server::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Deidara_Idle_Server(L"Idle", this));
	m_pStateMachine->Add_State(L"Appear", new CState_Deidara_Appear_Server(L"Appear", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_Deidara_HitMiddle_Server(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"Skill_C2Dragon", new CState_Deidara_Skill_C2Dragon_Server(L"Skill_C2Dragon", this));

	m_pStateMachine->Change_State(L"Appear");
}


CBoss_Deidara_Server* CBoss_Deidara_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Deidara_Server* pInstance = new CBoss_Deidara_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CBoss_Deidara" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Deidara_Server::Clone(void* pArg)
{
	CBoss_Deidara_Server* pInstance = new CBoss_Deidara_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CBoss_Deidara" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Deidara_Server::Free()
{
	__super::Free();
}
