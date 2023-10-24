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

	return S_OK;
}

void CPlayer_Naruto::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
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
	int i = 0;
	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		CPickingMgr::GetInstance()->Add_ColMesh(pOther->Get_Owner());
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

void CPlayer_Naruto::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Body_Collision(pOther->Get_Owner());
		return;
	}
}

void CPlayer_Naruto::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	int i = 0;

	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		CPickingMgr::GetInstance()->Delete_ColMesh(pOther->Get_Owner());
		return;
	}


	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Delete_CollisionStay(iColLayer, pOther);
		return;
	}
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
