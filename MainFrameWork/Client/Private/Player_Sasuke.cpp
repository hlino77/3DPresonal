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
#include "ColliderSphere.h"

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


	Set_Colliders();

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
	int i = 0;
}

void CPlayer_Sasuke::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{

}

void CPlayer_Sasuke::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	int i = 0;
}

void CPlayer_Sasuke::Set_Colliders()
{
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center();

	if (m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->IsActive())
		m_Coliders[(_uint)LAYER_COLLIDER::LAYER_ATTACK]->Set_Center();
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
