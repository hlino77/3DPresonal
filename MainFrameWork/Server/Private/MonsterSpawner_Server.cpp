#include "stdafx.h"
#include "GameInstance.h"
#include "MonsterSpawner_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "CollisionManager.h"
#include "RigidBody.h"
#include "NavigationMgr.h"
#include "Monster_Server.h"

CMonsterSpawner_Server::CMonsterSpawner_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"MonsterSpawner", OBJ_TYPE::SPAWNER)
{
}

CMonsterSpawner_Server::CMonsterSpawner_Server(const CMonsterSpawner_Server& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMonsterSpawner_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonsterSpawner_Server::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_uint* pObjectID = (_uint*)pArg;
	m_iObjectID = *pObjectID;
	m_fSpawnDelay = 1.0f;
	m_bSpawn = false;
    return S_OK;
}

void CMonsterSpawner_Server::Tick(_float fTimeDelta)
{
	if (m_bSpawn)
	{
		m_fSpawnDelay -= fTimeDelta;

		if (m_fSpawnDelay <= 0.0f)
		{
			Spawn_Monsters();
			m_bSpawn = false;
		}
	}
}

void CMonsterSpawner_Server::LateTick(_float fTimeDelta)
{
}

HRESULT CMonsterSpawner_Server::Render()
{

	return S_OK;
}

void CMonsterSpawner_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (m_bSpawn == false && pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
	{
		m_bSpawn = true;
		Set_Die();
	}
}

void CMonsterSpawner_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CMonsterSpawner_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	
}

void CMonsterSpawner_Server::Send_Collision(_uint iColLayer, CCollider* pOther, _bool bEnter)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddRef();


	Protocol::S_COLLISION pkt;

	pkt.set_benter(bEnter);

	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_ilayer(m_iLayer);
	pkt.set_iobjectid(m_iObjectID);
	pkt.set_icollayer(iColLayer);


	CGameObject* pOtherObject = pOther->Get_Owner();
	pkt.set_iotherid(pOtherObject->Get_ObjectID());
	pkt.set_iotherlayer(pOtherObject->Get_ObjectLayer());
	pkt.set_iothercollayer(pOther->Get_ColLayer());

	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);

	Safe_Release(pGameInstance);
}



void CMonsterSpawner_Server::Set_Spawner(Vec3 vPos, _float fRadius)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center(vPos);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(fRadius);
}

HRESULT CMonsterSpawner_Server::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	{
		CCollider::ColliderInfo tColliderInfo;
		tColliderInfo.m_bActive = true;
		tColliderInfo.m_iLayer = (_uint)LAYER_COLLIDER::LAYER_BODY;
		CSphereCollider* pCollider = nullptr;

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"), TEXT("Com_SphereColider"), (CComponent**)&pCollider, &tColliderInfo)))
			return E_FAIL;

		m_Coliders.emplace((_uint)LAYER_COLLIDER::LAYER_BODY, pCollider);
		CCollisionManager::GetInstance()->Add_Colider(pCollider);
	}


	Safe_Release(pGameInstance);


    return S_OK;
}

void CMonsterSpawner_Server::Spawn_Monsters()
{
	for (auto& Pos : m_MonsterPos)
	{
		Send_Monster(L"WhiteZetsu", Pos);
	}
	Set_Die();
}

void CMonsterSpawner_Server::Send_Monster(const wstring& szModelName, Vec3 vPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Protocol::S_CREATE_OBJCECT pkt;
	pkt.set_strname(CAsUtils::ToString(szModelName));
	pkt.set_iobjectid(g_iObjectID++);
	pkt.set_ilevel(pGameInstance->Get_CurrLevelIndex());
	pkt.set_ilayer((uint32)LAYER_TYPE::LAYER_MONSTER);
	pkt.set_iobjecttype((uint32)OBJ_TYPE::MONSTER);

	auto vPacketPos = pkt.mutable_vpos();
	vPacketPos->Resize(3, 0.0f);
	memcpy(vPacketPos->mutable_data(), &vPos, sizeof(Vec3));

	/*auto tMonster = pkt.add_tmonsterinfo();
	tMonster->set_ffollowdistance(10.0f);*/

	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);

	CMonster_Server::MODELDESC Desc;
	Desc.strFileName = CAsUtils::ToWString(pkt.strname());
	Desc.iObjectID = pkt.iobjectid();
	Desc.iLayer = pkt.ilayer();

	wstring szMonsterName = L"Prototype_GameObject_Monster_" + szModelName;
	CMonster_Server* pMonster = dynamic_cast<CMonster_Server*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), szMonsterName, &Desc));
	if (pMonster == nullptr)
		return;

	pMonster->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPos);
	pMonster->Set_FollowDistance(10.0f);
	CNavigationMgr::GetInstance()->Find_FirstCell(pMonster);

	Safe_Release(pGameInstance);
}


void CMonsterSpawner_Server::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_bDie = true;
}





void CMonsterSpawner_Server::Free()
{
	__super::Free();



	Safe_Release(m_pModelCom);

	Safe_Release(m_pTransformCom);
}

CMonsterSpawner_Server* CMonsterSpawner_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMonsterSpawner_Server* pInstance = new CMonsterSpawner_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CMonsterSpawner" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMonsterSpawner_Server::Clone(void* pArg)
{
	CMonsterSpawner_Server* pInstance = new CMonsterSpawner_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CMonsterSpawner" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}
