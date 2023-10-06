#include "stdafx.h"
#include "GameInstance.h"
#include "Monster_WhiteZetsu_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "State_WhiteZetsu_Idle_Server.h"
#include "State_WhiteZetsu_HitMiddle_Server.h"


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
	return S_OK;
}

void CMonster_WhiteZetsu_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (iColLayer == (_uint)LAYER_COLLIDER::LAYER_BODY && pOther->Get_ColLayer() == (_uint)LAYER_COLLIDER::LAYER_ATTACK)
	{
		Set_State(L"Hit_Middle");
	}
}

void CMonster_WhiteZetsu_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CMonster_WhiteZetsu_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
}

void CMonster_WhiteZetsu_Server::Send_MonsterInfo()
{
	Protocol::S_PLAYERINFO pkt;

	auto tPlayerInfo = pkt.add_tplayer();

	tPlayerInfo->set_iplayerid(m_iObjectID);
	tPlayerInfo->set_ilevel(CGameInstance::GetInstance()->Get_CurrLevelIndex());
	tPlayerInfo->set_ilayer((_uint)LAYER_TYPE::LAYER_MONSTER);


	auto vTargetPos = tPlayerInfo->mutable_vtargetpos();
	vTargetPos->Resize(3, 0.0f);
	Vec3 vPlayerTargetPos = m_vTargetPos.load();
	memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


	auto matWorld = tPlayerInfo->mutable_matworld();
	matWorld->Resize(16, 0.0f);
	Matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
	memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));


	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);
}


HRESULT CMonster_WhiteZetsu_Server::Ready_Components()
{
	__super::Ready_Components();


	Set_Colliders();
	return S_OK;
}

void CMonster_WhiteZetsu_Server::Ready_State()
{
	m_pStateMachine->Add_State(L"Idle", new CState_WhiteZetsu_Idle_Server(L"Idle", this));
	m_pStateMachine->Add_State(L"Hit_Middle", new CState_WhiteZetsu_HitMiddle_Server(L"Hit_Middle", this));
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
