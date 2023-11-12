#include "stdafx.h"
#include "Client_Defines.h"
#include "GameInstance.h"
#include "Player_Sasuke.h"
#include "Key_Manager.h"
#include "Camera_Player.h"
#include "State_Sasuke_Idle.h"
#include "State_Sasuke_RunLoop.h"
#include "State_Sasuke_RunEnd.h"
#include "State_Sasuke_Attack_cmb01.h"
#include "State_Sasuke_Attack_cmb03.h"
#include "State_Sasuke_Attack_cmb06.h"
#include "State_Sasuke_Attack_cmb08.h"
#include "ServerSessionManager.h"
#include "Engine_Defines.h"
#include "ColliderSphere.h"
#include "State_Sasuke_Jump.h"
#include "State_Sasuke_FallFront.h"
#include "State_Sasuke_DoubleJump.h"
#include "State_Sasuke_Land.h"
#include "State_Sasuke_WalkLoop.h"
#include "State_Sasuke_WalkEnd.h"
#include "ColliderOBB.h"
#include "CollisionManager.h"
#include "PickingMgr.h"
#include "State_Sasuke_WallLand.h"
#include "State_Sasuke_HitMiddle.h"
#include "State_Sasuke_HitSpinBlowUp.h"
#include "State_Sasuke_FallBehind.h"
#include "State_Sasuke_GetUp.h"
#include "State_Sasuke_DownToFloor.h"
#include "PhysXMgr.h"
#include "LineCircle.h"
#include "Pool.h"


CPlayer_Sasuke::CPlayer_Sasuke(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CPlayer(pDevice, pContext)
{
}

CPlayer_Sasuke::CPlayer_Sasuke(const CPlayer_Sasuke& rhs)
	: CPlayer(rhs)
{
}

HRESULT CPlayer_Sasuke::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer_Sasuke::Initialize(void* pArg)
{
	__super::Initialize(pArg);

	Ready_State();

	Ready_Coliders();

	m_fAttackMoveSpeed = 8.0f;


	return S_OK;
}

void CPlayer_Sasuke::Tick(_float fTimeDelta)
{
	m_pStateMachine->Tick_State(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CPlayer_Sasuke::LateTick(_float fTimeDelta)
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

HRESULT CPlayer_Sasuke::Render()
{
	__super::Render();

	if(m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->DebugRender();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->DebugRender();

	return S_OK;
}

void CPlayer_Sasuke::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (!m_bControl)
	{
		OnCollisionEnter_NoneControl(iColLayer, pOther);
		return;
	}
		

	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		if (m_bControl)
			CPickingMgr::GetInstance()->Add_ColMesh(pOther->Get_Owner());
		CPhysXMgr::GetInstance()->Add_ColMesh(m_iObjectID, pOther->Get_Owner()->Get_ModelName());
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
		{
			Set_SlowMotion(m_Coliders[iColLayer]->Get_SlowMotion());

			if (pOther->Get_AttackType() == (_uint)COLLIDER_ATTACK::MIDDLE)
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
		Hit_Attack(pOther);
		return;
	}

	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Add_CollisionStay(iColLayer, pOther);
		return;
	}


}

void CPlayer_Sasuke::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		Body_Collision(pOther->Get_Owner());
		return;
	}
}

void CPlayer_Sasuke::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	if (!m_bControl)
	{
		OnCollisionExit_NoneControl(iColLayer, pOther);
		return;
	}
		

	if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::COLMESH)
	{
		if (m_bControl)
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

void CPlayer_Sasuke::OnCollisionEnter_NoneControl(const _uint iColLayer, CCollider* pOther)
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
}

void CPlayer_Sasuke::OnCollisionExit_NoneControl(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_ATTACK && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_BODY)
	{
		if (pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::BOSS || pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::MONSTER)
		{
			m_bHitEffect = false;
		}
		return;
	}
}

void CPlayer_Sasuke::Set_Colliders(_float fTimeDelta)
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();
}

void CPlayer_Sasuke::Effect_Hit()
{
	Vec3 vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec3 vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
	vUp.Normalize();
	vLook.Normalize();
	vPos += vUp * 0.7f;
	vPos += vLook * 1.0f;

	
	/*Vec3 vColor(0.19f, 0.2f, 0.29f);
	vColor *= 0.5f;*/

	Vec3 vColor(0.19f, 0.0f, 0.69f);


	for (_uint i = 0; i < 50; ++i)
	{
		CLineCircle* pLineCircle = CPool<CLineCircle>::Get_Obj();
		if (pLineCircle)
		{
			pLineCircle->Appear(vLook, vPos, vColor, 1.0f);
		}
	}
}

void CPlayer_Sasuke::Send_PlayerInfo()
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


HRESULT CPlayer_Sasuke::Ready_Components()
{
	__super::Ready_Components();
	return S_OK;
}

HRESULT CPlayer_Sasuke::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_Sasuke_Idle(L"Idle", this));
	m_pStateMachine->Add_State(L"Run_Loop", new CState_Sasuke_RunLoop(L"Run_Loop", this));
	m_pStateMachine->Add_State(L"Run_End", new CState_Sasuke_RunEnd(L"Run_End", this));
	m_pStateMachine->Add_State(L"Jump", new CState_Sasuke_Jump(L"Jump", this));
	m_pStateMachine->Add_State(L"Fall_Front", new CState_Sasuke_FallFront(L"Fall_Front", this));
	m_pStateMachine->Add_State(L"DoubleJump", new CState_Sasuke_DoubleJump(L"DoubleJump", this));
	m_pStateMachine->Add_State(L"Land", new CState_Sasuke_Land(L"Land", this));
	m_pStateMachine->Add_State(L"Walk_Loop", new CState_Sasuke_WalkLoop(L"Walk_Loop", this));
	m_pStateMachine->Add_State(L"Walk_End", new CState_Sasuke_WalkEnd(L"Walk_End", this));
	m_pStateMachine->Add_State(L"WallLand", new CState_Sasuke_WallLand(L"WallLand", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_Sasuke_HitMiddle(L"Hit_Middle", this));
	m_pStateMachine->Add_State(L"Hit_SpinBlowUp", new CState_Sasuke_HitSpinBlowUp(L"Hit_SpinBlowUp", this));
	m_pStateMachine->Add_State(L"Fall_Behind", new CState_Sasuke_FallBehind(L"Fall_Behind", this));
	m_pStateMachine->Add_State(L"GetUp", new CState_Sasuke_GetUp(L"GetUp", this));
	m_pStateMachine->Add_State(L"DownToFloor", new CState_Sasuke_DownToFloor(L"DownToFloor", this));




	m_pStateMachine->Add_State(L"Attack_Normal_cmb01", new CState_Sasuke_Attack_cmb01(L"Attack_Normal_cmb01", this));
	m_pStateMachine->Add_State(L"Attack_Normal_cmb02", new CState_Sasuke_Attack_cmb03(L"Attack_Normal_cmb02", this));
	m_pStateMachine->Add_State(L"Attack_Normal_cmb03", new CState_Sasuke_Attack_cmb08(L"Attack_Normal_cmb03", this));
	m_pStateMachine->Add_State(L"Attack_Normal_cmb04", new CState_Sasuke_Attack_cmb06(L"Attack_Normal_cmb04", this));
	

	m_pStateMachine->Change_State(L"Idle");

	return S_OK;
}

HRESULT CPlayer_Sasuke::Ready_Coliders()
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


CPlayer_Sasuke* CPlayer_Sasuke::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer_Sasuke* pInstance = new CPlayer_Sasuke(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed To Created : CPlayer_Sasuke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CPlayer_Sasuke::Clone(void* pArg)
{
	CPlayer_Sasuke* pInstance = new CPlayer_Sasuke(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CPlayer_Sasuke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sasuke::Free()
{
	__super::Free();
}
