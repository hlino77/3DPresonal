#include "stdafx.h"
#include "GameInstance.h"
#include "Monster_WhiteZetsu_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "State_WhiteZetsu_Idle_Server.h"
#include "State_WhiteZetsu_HitMiddle_Server.h"
#include "State_WhiteZetsu_ChasePlayer_Server.h"
#include "State_WhiteZetsu_Appear_Server.h"
#include "State_WhiteZetsu_Attack_Normal_Server.h"
#include "State_WhiteZetsu_Attack_Kick_Server.h"
#include "State_WhiteZetsu_Attack_Punch_Server.h"
#include "State_WhiteZetsu_Dying_Normal_Server.h"
#include "State_WhiteZetsu_HitSpinBlowUp_Server.h"
#include "State_WhiteZetsu_GetUp_Server.h"
#include "State_WhiteZetsu_FallBehind_Server.h"
#include "State_WhiteZetsu_DownToFloor_Server.h"
#include "State_WhiteZetsu_HitSpinBlowDown_Server.h"
#include "State_WhiteZetsu_Die_Server.h"
#include "Skill_Server.h"


CMonster_WhiteZetsu_Server::CMonster_WhiteZetsu_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CMonster_Server(pDevice, pContext)
{
}

CMonster_WhiteZetsu_Server::CMonster_WhiteZetsu_Server(const CMonster_WhiteZetsu_Server& rhs)
	: CMonster_Server(rhs)
{
}

HRESULT CMonster_WhiteZetsu_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster_WhiteZetsu_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();

	m_iHp = 1000;

    return S_OK;
}

void CMonster_WhiteZetsu_Server::Tick(_float fTimeDelta)
{

	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMonster_WhiteZetsu_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_MonsterInfo();
	}
}

HRESULT CMonster_WhiteZetsu_Server::Render()
{
	__super::Render();

	return S_OK;
}

void CMonster_WhiteZetsu_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
			Set_SlowMotion(m_Coliders[iColLayer]->Get_SlowMotion());
		return;
	}

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

void CMonster_WhiteZetsu_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Body_Collision(pOther->Get_Owner());
	}
}

void CMonster_WhiteZetsu_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Delete_CollisionStay(iColLayer, pOther);
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		CGameObject* pOwner = pOther->Get_Owner();

		_uint iObjType = pOwner->Get_ObjectType();

		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
			Set_SlowMotion(false);
		else if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::SKILL)
		{
			_uint iSkillOwnerType = dynamic_cast<CSkill_Server*>(pOwner)->Get_SkillOwner()->Get_ObjectType();
			if (iSkillOwnerType == OBJ_TYPE::PLAYER)
				Set_SlowMotion(false);
		}
		
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
			Set_SlowMotion(false);
		return;
	}
}

void CMonster_WhiteZetsu_Server::Send_MonsterInfo()
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


void CMonster_WhiteZetsu_Server::Update_NearTarget(_float fTimeDelta)
{
	m_fFindTargetTime += fTimeDelta;
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
	}

}

void CMonster_WhiteZetsu_Server::Attack_Random()
{
	_uint iAttack = rand() % 2;

	switch (iAttack)
	{
	case 0:
		Set_State(L"Attack_Normal");
		break;
	case 1:
		Set_State(L"Attack_Kick");
		break;
	}
}

HRESULT CMonster_WhiteZetsu_Server::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

void CMonster_WhiteZetsu_Server::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_WhiteZetsu_Idle_Server(L"Idle", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_WhiteZetsu_HitMiddle_Server(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"ChaseTarget", new CState_WhiteZetsu_ChasePlayer_Server(L"ChaseTarget", this));
	m_pStateMachine->Add_State(L"Appear", new CState_WhiteZetsu_Appear_Server(L"Appear", this));
	m_pStateMachine->Add_State(L"Attack_Normal", new CState_WhiteZetsu_Attack_Normal_Server(L"Attack_Normal", this));
	m_pStateMachine->Add_State(L"Attack_Kick", new CState_WhiteZetsu_Attack_Kick_Server(L"Attack_Kick", this));
	m_pStateMachine->Add_State(L"Attack_Punch", new CState_WhiteZetsu_Attack_Punch_Server(L"Attack_Punch", this));
	m_pStateMachine->Add_State(L"Dying_Normal", new CState_WhiteZetsu_Dying_Normal_Server(L"Dying_Normal", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_WhiteZetsu_HitSpinBlowUp_Server(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_WhiteZetsu_GetUp_Server(L"GetUp", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_WhiteZetsu_FallBehind_Server(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_WhiteZetsu_DownToFloor_Server(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowDown", new CState_WhiteZetsu_HitSpinBlowDown_Server(L"Hit_SpinBlowDown", this));
	m_pStateMachine->Add_State(L"Die", new CState_WhiteZetsu_Die_Server(L"Die", this));




	m_pStateMachine->Change_State(L"Appear");
}

CMonster_WhiteZetsu_Server* CMonster_WhiteZetsu_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonster_WhiteZetsu_Server* pInstance = new CMonster_WhiteZetsu_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CMonster_WhiteZetsu" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonster_WhiteZetsu_Server::Clone(void* pArg)
{
	CMonster_WhiteZetsu_Server* pInstance = new CMonster_WhiteZetsu_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CMonster_WhiteZetsu" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonster_WhiteZetsu_Server::Free()
{
	__super::Free();
}
