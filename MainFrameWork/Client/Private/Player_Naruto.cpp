#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Player_Naruto.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "State_Naruto_Idle.h"
#include "State_Naruto_RunLoop.h"
#include "State_Naruto_RunEnd.h"
#include "State_Naruto_Attack_Punch_Left.h"
#include "State_Naruto_Attack_Punch_Right.h"
#include "State_Naruto_Attack_ElbowStrike.h"
#include "State_Naruto_Attack_JumpDoubleKick.h"
#include "ServerSessionManager.h"
#include "ColliderSphere.h"
#include "State_Naruto_Jump.h"
#include "State_Naruto_FallFront.h"
#include "State_Naruto_DoubleJump.h"
#include "State_Naruto_Land.h"
#include "State_Naruto_WalkLoop.h"
#include "State_Naruto_WalkEnd.h"
#include "PickingMgr.h"
#include "ColliderOBB.h"
#include "State_Naruto_WallLand.h"
#include "State_Naruto_HitMiddle.h"
#include "State_Naruto_HitSpinBlowUp.h"
#include "State_Naruto_FallBehind.h"
#include "State_Naruto_DownToFloor.h"
#include "State_Naruto_GetUp.h"
#include "LineCircle.h"
#include "PhysXMgr.h"
#include "Pool.h"
#include "RasenganCircle.h"
#include "State_Naruto_Skill_Rasengun_Charging.h"
#include "State_Naruto_Skill_Rasengun_Loop.h"
#include "State_Naruto_Skill_Rasengun_Start.h"
#include "State_Naruto_Skill_Rasengun_Attack.h"
#include "State_Naruto_Skill_Rasengun_RunLoop.h"
#include "State_Naruto_Skill_Rasengun_RunStart.h"
#include "Skill_Rasengun.h"
#include "Skill_RasenSyuriken.h"
#include "State_Naruto_Skill_RasenSyuriken.h"

CPlayer_Naruto::CPlayer_Naruto(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPlayer(pDevice, pContext)
{
}

CPlayer_Naruto::CPlayer_Naruto(const CPlayer_Naruto& rhs)
	: CPlayer(rhs)
{
}

HRESULT CPlayer_Naruto::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Naruto::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();

	Ready_Coliders();

	m_fAttackMoveSpeed = 8.0f;

	
	Send_MakeSkill(L"Rasengun");

	Send_MakeSkill(L"RasenSyuriken");


	return S_OK;
}

void CPlayer_Naruto::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);




	//Vec3 vPos = m_vTargetPos.load();

	//cout << vPos.x << "  " << vPos.y << "  " << vPos.z << endl;
}

void CPlayer_Naruto::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	Set_Colliders(fTimeDelta);

	if (m_bControl)
	{
		m_fSendInfoTime += fTimeDelta;
		if (m_fSendInfoTime >= 0.05f)
		{
			m_fSendInfoTime = 0.0f;
			Send_PlayerInfo();
		}
	}
}

HRESULT CPlayer_Naruto::Render()
{
	__super::Render();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->DebugRender();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->DebugRender();

	return S_OK;
}

void CPlayer_Naruto::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (!m_bControl)
	{
		OnCollisionEnter_NoneControl(iColLayer, pOther);
		return;
	}
		

	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		if(m_bControl)
			CPickingMgr::GetInstance()->Add_ColMesh(pOther->Get_Owner());
		CPhysXMgr::GetInstance()->Add_ColMesh(m_iObjectID, pOther->Get_Owner()->Get_ModelName());
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
		{
			Set_SlowMotion(m_Coliders[iColLayer]->Get_SlowMotion());

			if(pOther->Get_AttackType() == (_uint)COLLIDER_ATTACK::MIDDLE)
				m_pCamera->Cam_Shake(0.001f, 0.1f);
			else if (pOther->Get_AttackType() == (_uint)COLLIDER_ATTACK::SPINBLOWDOWN)
				m_pCamera->Cam_Shake(0.002f, 0.15f);

			Add_Hit();

			if (m_bHitEffect == false)
			{
				Effect_Hit();
				m_bHitEffect = true;
			}
		}
		return;
	}


	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		if (m_bInvincible == false)
			Hit_Attack(pOther);
		return;
	}


	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Add_CollisionStay(iColLayer, pOther);

		return;
	}
}

void CPlayer_Naruto::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS)
			m_bEnemyBodyHit = true;

		Body_Collision(pOther->Get_Owner());
		return;
	}
}

void CPlayer_Naruto::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	if (!m_bControl)
	{
		OnCollisionExit_NoneControl(iColLayer, pOther);
		return;
	}
		

	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		if(m_bControl)
			CPickingMgr::GetInstance()->Delete_ColMesh(pOther->Get_Owner());
		CPhysXMgr::GetInstance()->Delete_ColMesh(m_iObjectID, pOther->Get_Owner()->Get_ModelName());
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
		{
			Set_SlowMotion(false);
			m_bHitEffect = false;
		}
		return;
	}


	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Delete_CollisionStay(iColLayer, pOther);
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
			Set_SlowMotion(false);
		return;
	}
}

void CPlayer_Naruto::OnCollisionEnter_NoneControl(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
		{
			if (m_bHitEffect == false)
			{
				Effect_Hit();
				m_bHitEffect = true;
			}
		}
		return;
	}

	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		if (m_bControl)
			CPickingMgr::GetInstance()->Add_ColMesh(pOther->Get_Owner());
		CPhysXMgr::GetInstance()->Add_ColMesh(m_iObjectID, pOther->Get_Owner()->Get_ModelName());
		return;
	}
}

void CPlayer_Naruto::OnCollisionExit_NoneControl(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
		{
			m_bHitEffect = false;
		}
		return;
	}


	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		if (m_bControl)
			CPickingMgr::GetInstance()->Delete_ColMesh(pOther->Get_Owner());
		CPhysXMgr::GetInstance()->Delete_ColMesh(m_iObjectID, pOther->Get_Owner()->Get_ModelName());
		return;
	}
}

void CPlayer_Naruto::Set_Skill(CGameObject* pGameObject)
{
	WRITE_LOCK
	if (pGameObject->Get_ModelName() == L"Rasengun")
		m_pRasengun = dynamic_cast<CSkill_Rasengun*>(pGameObject);
	else if (pGameObject->Get_ModelName() == L"RasenSyuriken")
		m_pRasenSyuriken = dynamic_cast<CSkill_RasenSyuriken*>(pGameObject);
}

void CPlayer_Naruto::Send_PlayerInfo()
{
	Protocol::S_OBJECTINFO pkt;

	auto tPlayerInfo = pkt.add_tobject();

	tPlayerInfo->set_iobjectid(m_iObjectID);
	tPlayerInfo->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tPlayerInfo->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);


	auto vTargetPos = tPlayerInfo->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tPlayerInfo->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);
}

void CPlayer_Naruto::Set_Colliders(_float fTimeDelta)
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();
}


void CPlayer_Naruto::Effect_Hit()
{
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();
	vLook.Normalize();
	vPos += vUp * 0.7f;
	vPos += vLook * 1.0f;


	//Vec3 vColor(0.93f, 0.41f, 0.05f);

	//Vec3 vColor(0.63f, 0.11f, 0.0f);

	Vec4 vColor(1.0f, 1.0f, 1.0f, 1.0f);
	Vec4 vBlurColor(0.99f, 0.41f, 0.094f, 1.0f);
	
	//Vec3 vColor(1.0f , 0.0f, 0.0f);

	for (_uint i = 0; i < 50; ++i)
	{
		CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
		if (pLineCircle)
		{
			pLineCircle->Appear(vPos, vColor, vBlurColor, 1.0f);
		}
	}
}

HRESULT CPlayer_Naruto::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Naruto_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Run_Loop", new CState_Naruto_RunLoop(L"Run_Loop", this));
	m_pStateMachine->Add_State(L"Run_End", new CState_Naruto_RunEnd(L"Run_End", this));
	m_pStateMachine->Add_State(L"Jump", new CState_Naruto_Jump(L"Jump", this));
	m_pStateMachine->Add_State(L"Fall_Front", new CState_Naruto_FallFront(L"Fall_Front", this));
	m_pStateMachine->Add_State(L"DoubleJump", new CState_Naruto_DoubleJump(L"DoubleJump", this));
	m_pStateMachine->Add_State(L"Land", new CState_Naruto_Land(L"Land", this));
	m_pStateMachine->Add_State(L"Walk_Loop", new CState_Naruto_WalkLoop(L"Walk_Loop", this));
	m_pStateMachine->Add_State(L"Walk_End", new CState_Naruto_WalkEnd(L"Walk_End", this));
	m_pStateMachine->Add_State(L"WallLand", new CState_Naruto_WallLand(L"WallLand", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_Naruto_HitMiddle(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_Naruto_HitSpinBlowUp(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_Naruto_FallBehind(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_Naruto_DownToFloor(L"DownToFloor", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_Naruto_GetUp(L"GetUp", this));

	m_pStateMachine->Add_State(L"Rasengun_Start", new CState_Naruto_Skill_Rasengun_Start(L"Rasengun_Start", this));
	m_pStateMachine->Add_State(L"Rasengun_Charge", new CState_Naruto_Skill_Rasengun_Charging(L"Rasengun_Charge", this));
	m_pStateMachine->Add_State(L"Rasengun_Loop", new CState_Naruto_Skill_Rasengun_Loop(L"Rasengun_Loop", this));
	m_pStateMachine->Add_State(L"Rasengun_RunStart", new CState_Naruto_Skill_Rasengun_RunStart(L"Rasengun_RunStart", this));
	m_pStateMachine->Add_State(L"Rasengun_RunLoop", new CState_Naruto_Skill_Rasengun_RunLoop(L"Rasengun_RunLoop", this));
	m_pStateMachine->Add_State(L"Rasengun_Attack", new CState_Naruto_Skill_Rasengun_Attack(L"Rasengun_Attack", this));

	m_pStateMachine->Add_State(L"RasenSyuriken", new CState_Naruto_Skill_RasenSyuriken(L"RasenSyuriken", this));


	m_pStateMachine->Add_State(L"Attack_Normal_cmb01", new CState_Naruto_Attack_Punch_Left(L"Attack_Normal_cmb01", this));
	m_pStateMachine->Add_State(L"Attack_Normal_cmb02", new CState_Naruto_Attack_Punch_Right(L"Attack_Normal_cmb02", this));
	m_pStateMachine->Add_State(L"Attack_Normal_cmb03", new CState_Naruto_Attack_ElbowStrike(L"Attack_Normal_cmb03", this));
	m_pStateMachine->Add_State(L"Attack_Normal_cmb04", new CState_Naruto_Attack_JumpDoubleKick(L"Attack_Normal_cmb04", this));
	

	m_pStateMachine->Change_State(L"Idle");

	return S_OK;
}

HRESULT CPlayer_Naruto::Ready_Coliders()
{
	m_BoneIndex.emplace(L"RightHand", m_pModelCom->Find_BoneIndex(L"RightHandMiddle3_end"));

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->SetActive(true);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(1.0f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Offset(Vec3(0.0f, 0.7f, 0.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_BODY);



	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Radius(0.5f);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->SetActive(false);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Offset(Vec3(0.0f, 0.7f, 1.0f));
	Send_ColliderState((_uint)LAYER_COLLIDER::LAYER_ATTACK);



	return S_OK;
}



CPlayer_Naruto* CPlayer_Naruto::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Naruto::Clone(void* pArg)
{
	CPlayer_Naruto* pInstance = new CPlayer_Naruto(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer_Naruto");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Naruto::Free()
{
	__super::Free();
}
