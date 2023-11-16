#include "stdafx.h"
#include "GameInstance.h"
#include "BossSpawner_Server.h"
#include "AsUtils.h"
#include "ColliderSphere.h"
#include "GameSessionManager.h"
#include "CollisionManager.h"
#include "RigidBody.h"
#include "NavigationMgr.h"
#include "Monster_Server.h"
#include "Boss_Server.h"

CBossSpawner_Server::CBossSpawner_Server(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext, L"BossSpawner", OBJ_TYPE::SPAWNER)
{
}

CBossSpawner_Server::CBossSpawner_Server(const CBossSpawner_Server& rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossSpawner_Server::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBossSpawner_Server::Initialize(void* pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	_uint* pObjectID = (_uint*)pArg;
	m_iObjectID = *pObjectID;
	m_fSpawnDelay = 1.0f;
	m_bSpawn = false;
	m_iLayer = (_uint)LAYER_TYPE::LAYER_BACKGROUND;

    return S_OK;
}

void CBossSpawner_Server::Tick(_float fTimeDelta)
{
	if (m_bSpawn)
	{
		m_fSpawnDelay -= fTimeDelta;

		if (m_fSpawnDelay <= 0.0f)
		{
			Spawn_Boss();
			m_bSpawn = false;
		}
	}
}

void CBossSpawner_Server::LateTick(_float fTimeDelta)
{
}

HRESULT CBossSpawner_Server::Render()
{

	return S_OK;
}

void CBossSpawner_Server::OnCollisionEnter(const _uint iColLayer, CCollider* pOther)
{
	if (m_bSpawn == false && pOther->Get_Owner()->Get_ObjectType() == OBJ_TYPE::PLAYER)
	{
		m_bSpawn = true;
		Set_Die();

		Send_Collision(iColLayer, pOther, true);
	}
}

void CBossSpawner_Server::OnCollisionStay(const _uint iColLayer, CCollider* pOther)
{
}

void CBossSpawner_Server::OnCollisionExit(const _uint iColLayer, CCollider* pOther)
{
	
}

void CBossSpawner_Server::Send_Collision(_uint iColLayer, CCollider* pOther, _bool bEnter)
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



void CBossSpawner_Server::Set_Spawner(Vec3 vPos, _float fRadius)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Center(vPos);
	m_Coliders[(_uint)LAYER_COLLIDER::LAYER_BODY]->Set_Radius(fRadius);
}

void CBossSpawner_Server::Spawn_Boss()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Vec3 vPos = Vec3(80.10f, 27.94f, -65.99f);

	{
		Protocol::S_CREATE_OBJCECT pkt;
		pkt.set_strname(CAsUtils::ToString(L"Madara"));
		pkt.set_iobjectid(g_iObjectID++);
		pkt.set_ilevel((uint32)LEVELID::LEVEL_KONOHA);
		pkt.set_ilayer((uint32)LAYER_TYPE::LAYER_BOSS);
		pkt.set_iobjecttype((uint32)OBJ_TYPE::BOSS);

		auto vPacketPos = pkt.mutable_vpos();
		vPacketPos->Resize(3, 0.0f);
		memcpy(vPacketPos->mutable_data(), &vPos, sizeof(Vec3));

		/*auto tMonster = pkt.add_tmonsterinfo();
		tMonster->set_ffollowdistance(10.0f);*/

		SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
		CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);


		CBoss_Server::MODELDESC Desc;
		Desc.strFileName = CAsUtils::ToWString(pkt.strname());
		Desc.iObjectID = pkt.iobjectid();
		Desc.iLayer = pkt.ilayer();

		wstring szMonsterName = L"Prototype_GameObject_Boss_Madara";
		CBoss_Server* pBoss = dynamic_cast<CBoss_Server*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), szMonsterName, &Desc));
		if (pBoss == nullptr)
			return;

		pBoss->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPos);
		pBoss->Set_Skill(nullptr);
		CNavigationMgr::GetInstance()->Find_FirstCell(pBoss);
	}


	Safe_Release(pGameInstance);


}

HRESULT CBossSpawner_Server::Ready_Components()
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



void CBossSpawner_Server::Set_Die()
{
	for (auto& Collider : m_Coliders)
		Collider.second->SetActive(false);

	m_bDie = true;
}





void CBossSpawner_Server::Free()
{
	__super::Free();



	Safe_Release(m_pModelCom);

	Safe_Release(m_pTransformCom);
}

CBossSpawner_Server* CBossSpawner_Server::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CBossSpawner_Server* pInstance = new CBossSpawner_Server(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		cout << "Failed To Created : CBossSpawner_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CBossSpawner_Server::Clone(void* pArg)
{
	CBossSpawner_Server* pInstance = new CBossSpawner_Server(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		cout << "Failed To Cloned : CBossSpawner_Server" << endl;
		Safe_Release(pInstance);
	}

	return pInstance;
}
