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
#include "State_Deidara_GetUp_Server.h"
#include "State_Deidara_DownToFloor_Server.h"
#include "State_Deidara_FallBehind_Server.h"
#include "State_Deidara_HitSpinBlowDown_Server.h"
#include "State_Deidara_HitSpinBlowUp_Server.h"
#include "State_Deidara_Dying_Normal_Server.h"
#include "State_Deidara_Die_Server.h"
#include "State_Deidara_ChasePlayer_Server.h"
#include "State_Deidara_Attack_cmb01_Server.h"
#include "State_Deidara_Attack_cmb02_Server.h"
#include "Skill_TwinBird_Server.h"
#include "State_Deidara_Skill_TwinBirds_Server.h"


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

	m_iHp = 1;

	m_fFollowDistance = 40.0f;

	m_fAttackMoveSpeed = 8.0f;
	

	SKILLINFO tC2Dragon;
	tC2Dragon.m_bReady = true;
	tC2Dragon.m_fCoolTime = 90.0f;
	tC2Dragon.m_fCurrCoolTime = 0.0f;

	m_SkillInfo.push_back(tC2Dragon);


	SKILLINFO tTwinBirds;
	tTwinBirds.m_bReady = true;
	tTwinBirds.m_fCoolTime = 20.0f;
	tTwinBirds.m_fCurrCoolTime = 0.0f;

	m_SkillInfo.push_back(tTwinBirds);


	m_pTwinBird[0] = nullptr;
	m_pTwinBird[1] = nullptr;



    return S_OK;
}

void CBoss_Deidara_Server::Tick(_float fTimeDelta)
{
	for (auto& Skill : m_SkillInfo)
	{
		Update_Skill(Skill, fTimeDelta);
	}


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

void CBoss_Deidara_Server::Set_Skill(CGameObject* pObject)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	Send_MakeSkill(L"TwinBird", (CGameObject**)&m_pTwinBird[0]);
	Send_MakeSkill(L"TwinBird", (CGameObject**)&m_pTwinBird[1]);

	Safe_Release(pGameInstance);
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
	m_fFindTargetTime += fTimeDelta;

	if (m_fFindTargetTime >= 2.0f)
	{
		Find_NearTarget();
		if (m_pNearTarget)
		{
			_float fDistance = Get_NearTargetDistance();

			if (fDistance <= 3.0f)
			{
				Set_State(L"Attack_cmb01");
			}
			else if (fDistance <= m_fFollowDistance)
			{
				if (m_pStateMachine->Get_CurrState() != L"ChaseTarget" && fDistance > 3.0f)
					Set_State(L"ChaseTarget");

				Send_NearTarget();
			}
		}
		m_fFindTargetTime = 0.0f;
	}

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


	m_SkillInfo[DEIDARA_SKILL::C2DRAGON].m_bReady = false;
}

void CBoss_Deidara_Server::Shoot_TwinBirds()
{
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	Vec3 vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	

	vRight.Normalize();
	vLook.Normalize();
	vUp.Normalize();

	{
		Vec3 vTargetPos = vPos + (vLook * 0.5f) + (vRight * 0.5f) + (vUp * 2.5f);
		Vec3 vTargetLook = XMVector3Rotate(vLook, Quaternion::CreateFromAxisAngle(vUp, XMConvertToRadians(30.0f)));

		m_pTwinBird[0]->Shoot_TwinBird(m_pNearTarget, vTargetPos, vTargetLook);
	}

	{
		Vec3 vTargetPos = vPos + (vLook * 0.5f) + (vRight * -0.5f) + (vUp * 1.5f);
		Vec3 vTargetLook = XMVector3Rotate(vLook, Quaternion::CreateFromAxisAngle(vUp, XMConvertToRadians(-30.0f)));

		m_pTwinBird[1]->Shoot_TwinBird(m_pNearTarget, vTargetPos, vTargetLook);
	}

	m_SkillInfo[DEIDARA_SKILL::TWINBIRD].m_bReady = false;
}

void CBoss_Deidara_Server::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_pC2Dragon->Set_Die();
	m_pTwinBird[0]->Set_Die();
	m_pTwinBird[1]->Set_Die();


	m_bDie = true;
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
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_Deidara_HitSpinBlowUp_Server(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowDown", new CState_Deidara_HitSpinBlowDown_Server(L"Hit_SpinBlowDown", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_Deidara_GetUp_Server(L"GetUp", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_Deidara_DownToFloor_Server(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_Deidara_FallBehind_Server(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"Dying_Normal", new CState_Deidara_Dying_Normal_Server(L"Dying_Normal", this));
	m_pStateMachine->Add_State(L"Die", new CState_Deidara_Die_Server(L"Die", this));
	m_pStateMachine->Add_State(L"ChaseTarget", new CState_Deidara_ChasePlayer_Server(L"ChaseTarget", this));
	m_pStateMachine->Add_State(L"Attack_cmb01", new CState_Deidara_Attack_cmb01_Server(L"Attack_cmb01", this));
	m_pStateMachine->Add_State(L"Attack_cmb02", new CState_Deidara_Attack_cmb02_Server(L"Attack_cmb02", this));
	m_pStateMachine->Add_State(L"Skill_TwinBirds", new CState_Deidara_Skill_TwinBirds_Server(L"Skill_TwinBirds", this));


	m_pStateMachine->Change_State(L"Appear");
}

void CBoss_Deidara_Server::Update_Skill(SKILLINFO& tSkill, _float fTimeDelta)
{
	if (tSkill.m_bReady == false)
	{
		tSkill.m_fCurrCoolTime += fTimeDelta;

		if (tSkill.m_fCurrCoolTime > tSkill.m_fCoolTime)
		{
			tSkill.m_fCurrCoolTime = 0.0f;
			tSkill.m_bReady = true;
		}
	}
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
