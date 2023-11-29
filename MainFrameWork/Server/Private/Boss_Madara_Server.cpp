#include "stdafx.h"
#include "GameInstance.h"
#include "Boss_Madara_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "State_Madara_Appear_Server.h"
#include "State_Madara_Idle_Server.h"
#include "State_Madara_DownToFloor_Server.h"
#include "State_Madara_FallBehind_Server.h"
#include "State_Madara_GetUp_Server.h"
#include "State_Madara_HitMiddle_Server.h"
#include "State_Madara_HitSpinBlowDown_Server.h"
#include "State_Madara_HitSpinBlowUp_Server.h"
#include "NavigationMgr.h"
#include "State_Madara_TeleportAttack_Server.h"
#include "State_Madara_Attack_Punch_Server.h"
#include "State_Madara_Attack_TurnKick_Server.h"
#include "State_Madara_Attack_DoubleTurnKick_Server.h"
#include "Skill_Server.h"
#include "Skill_Meteor_Server.h"
#include "State_Madara_Skill_Meteor_Server.h"
#include "EventMgr.h"
#include "Skill_MadaraFireBall_Server.h"
#include "State_Madara_Skill_FireBall_End_Server.h"
#include "State_Madara_Skill_FireBall_Loop_Server.h"
#include "State_Madara_Skill_FireBall_Start_Server.h"
#include "State_Madara_Dying_Normal_Server.h"
#include "State_Madara_Die_Server.h"


CBoss_Madara_Server::CBoss_Madara_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBoss_Server(pDevice, pContext)
{
}

CBoss_Madara_Server::CBoss_Madara_Server(const CBoss_Madara_Server& rhs)
	: CBoss_Server(rhs)
{
}

HRESULT CBoss_Madara_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoss_Madara_Server::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();

	m_iHp = 200;
	m_iMaxHp = 200;

	m_fFollowDistance = 40.0f;

	m_fAttackMoveSpeed = 12.0f;

	m_fTeleportAttackDelay = 5.0f;

	CNavigationMgr::GetInstance()->Reset_Navigation();
	CNavigationMgr::GetInstance()->Add_Navigation(L"Madara.navi");



	SKILLINFO tMeteor;
	tMeteor.m_bReady = false;
	tMeteor.m_fCoolTime = 60.0f;
	tMeteor.m_fCurrCoolTime = 0.0f;

	m_SkillInfo.push_back(tMeteor);


	SKILLINFO tFireBall;
	tFireBall.m_bReady = false;
	tFireBall.m_fCoolTime = 27.0f;
	tFireBall.m_fCurrCoolTime = 15.0f;

	m_SkillInfo.push_back(tFireBall);



    return S_OK;
}

void CBoss_Madara_Server::Tick(_float fTimeDelta)
{
	for (auto& Skill : m_SkillInfo)
	{
		Update_Skill(Skill, fTimeDelta);
	}


	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);


	if (m_SkillInfo[MADARA_SKILL::METEOR].m_bReady)
	{
		if (m_bFireBall == false)
		{
			CEventMgr::GetInstance()->Start_Event((_uint)EVENT::MADARAMETEOR);
			m_SkillInfo[MADARA_SKILL::METEOR].m_bReady = false;
		}
	}

}

void CBoss_Madara_Server::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_fSendInfoTime += fTimeDelta;
	if (m_fSendInfoTime >= 0.05f)
	{
		m_fSendInfoTime = 0.0f;
		Send_BossInfo();
	}
}

HRESULT CBoss_Madara_Server::Render()
{
	__super::Render();

	return S_OK;
}

void CBoss_Madara_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
		{
			Set_SlowMotion(m_Coliders[iColLayer]->Get_SlowMotion());
			m_bNormalAttackHit = true;
			Send_Collision(iColLayer, pOther, true);
		}
			
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		if(m_bInvincible == false)
			Hit_Attack(pOther);
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Add_CollisionStay(iColLayer, pOther);
		return;
	}
}

void CBoss_Madara_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Body_Collision(pOther->Get_Owner());
	}
}

void CBoss_Madara_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
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
		{
			Set_SlowMotion(false);
			Send_Collision(iColLayer, pOther, false);
		}
		return;
	}

}

void CBoss_Madara_Server::Set_Skill(CGameObject* pObject)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	
	Send_MakeSkill(L"Meteor", (CGameObject**)&m_pMeteor);
	Send_MakeSkill(L"MadaraFireBall", (CGameObject**)&m_pFireBall);

	Safe_Release(pGameInstance);
}

void CBoss_Madara_Server::Send_BossInfo()
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


void CBoss_Madara_Server::Update_NearTarget(_float fTimeDelta)
{
	/*m_fFindTargetTime += fTimeDelta;

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
	}*/

}

_bool CBoss_Madara_Server::Ready_TeleportAttack(_float fTimeDelta)
{
	m_fTeleportAttackDelay -= fTimeDelta;

	if (m_fTeleportAttackDelay <= 0.0f)
	{
		Find_NearTarget();
		Vec3 vPlayerPos = m_pNearTarget->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		Vec3 vDir = vPos - vPlayerPos;

		vDir.Normalize();

		vDir *= 0.5f;

		Vec3 vTargetPos = vPlayerPos + vDir;


		_int iCheckCell = CNavigationMgr::GetInstance()->Check_Pos_InCell(vTargetPos);
		if (iCheckCell != -1)
		{
			m_iCurrCell = iCheckCell;
			m_vTargetPos = vTargetPos;
			m_fTeleportAttackDelay = 5.0f;
			Send_NearTarget();
			Set_State(L"TeleportAttack");
			return true;
		}
		m_fTeleportAttackDelay = 1.0f;
	}

	return false;
}

void CBoss_Madara_Server::Attack_Random()
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

void CBoss_Madara_Server::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_bDie = true;
}

void CBoss_Madara_Server::ResetSkill(MADARA_SKILL eSkill)
{
	m_SkillInfo[eSkill].m_fCurrCoolTime = 0.0f;
	m_SkillInfo[eSkill].m_bReady = false;
}


HRESULT CBoss_Madara_Server::Ready_Components()
{
	__super::Ready_Components();

	return S_OK;
}

void CBoss_Madara_Server::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Madara_Idle_Server(L"Idle", this));
	m_pStateMachine->Add_State(L"Appear", new CState_Madara_Appear_Server(L"Appear", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_Madara_HitMiddle_Server(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_Madara_HitSpinBlowUp_Server(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowDown", new CState_Madara_HitSpinBlowDown_Server(L"Hit_SpinBlowDown", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_Madara_GetUp_Server(L"GetUp", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_Madara_DownToFloor_Server(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_Madara_FallBehind_Server(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"TeleportAttack", new CState_Madara_TeleportAttack_Server(L"TeleportAttack", this));
	m_pStateMachine->Add_State(L"Attack_Punch", new CState_Madara_Attack_Punch_Server(L"Attack_Punch", this));
	m_pStateMachine->Add_State(L"Attack_TurnKick", new CState_Madara_Attack_TurnKick_Server(L"Attack_TurnKick", this));
	m_pStateMachine->Add_State(L"Attack_DoubleTurnKick", new CState_Madara_Attack_DoubleTurnKick_Server(L"Attack_DoubleTurnKick", this));
	m_pStateMachine->Add_State(L"Skill_Meteor", new CState_Madara_Skill_Meteor_Server (L"Skill_Meteor", this));

	m_pStateMachine->Add_State(L"Skill_FireBall_Start", new CState_Madara_Skill_FireBall_Start_Server(L"Skill_FireBall_Start", this));
	m_pStateMachine->Add_State(L"Skill_FireBall_Loop", new CState_Madara_Skill_FireBall_Loop_Server(L"Skill_FireBall_Loop", this));
	m_pStateMachine->Add_State(L"Skill_FireBall_End", new CState_Madara_Skill_FireBall_End_Server(L"Skill_FireBall_End", this));

	m_pStateMachine->Add_State(L"Dying_Normal", new CState_Madara_Dying_Normal_Server(L"Dying_Normal", this));
	m_pStateMachine->Add_State(L"Die", new CState_Madara_Die_Server(L"Die", this));

	m_pStateMachine->Change_State(L"Appear");
}

void CBoss_Madara_Server::Update_Skill(SKILLINFO& tSkill, _float fTimeDelta)
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


CBoss_Madara_Server* CBoss_Madara_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBoss_Madara_Server* pInstance = new CBoss_Madara_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CBoss_Madara_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBoss_Madara_Server::Clone(void* pArg)
{
	CBoss_Madara_Server* pInstance = new CBoss_Madara_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CBoss_Madara_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Madara_Server::Free()
{
	__super::Free();
}
