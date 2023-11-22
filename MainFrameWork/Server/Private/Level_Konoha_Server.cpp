#include "stdafx.h"
#include "Level_Konoha_Server.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "AsUtils.h"
#include "Player_Server.h"
#include "GameInstance.h"
#include "Monster_WhiteZetsu_Server.h"
#include "CollisionManager.h"
#include "ThreadManager.h"
#include "Struct.pb.h"
#include "NavigationMgr.h"
#include "Boss_Server.h"
#include "EventMgr.h"
#include "ServerEvent_ArenaStart.h"
#include "ServerEvent_PlayerStart.h"
#include "ServerEvent_BattleStart.h"
#include "AsFileUtils.h"
#include <filesystem>
#include "MonsterSpawner_Server.h"
#include "BossSpawner_Server.h"
#include "ServerEvent_MadaraMeteor.h"


CLevel_Konoha_Server::CLevel_Konoha_Server()
	: CLevel(nullptr, nullptr)
{
}

HRESULT CLevel_Konoha_Server::Initialize()
{
	CNavigationMgr::GetInstance()->Add_Navigation(L"Kono.navi");

	Ready_Events();


	Broadcast_LevelState(LEVELSTATE::INITREADY);
	Wait_ClientLevelState(LEVELSTATE::INITREADY);
	Broadcast_LevelState(LEVELSTATE::INITSTART);

	


	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(LAYER_TYPE::LAYER_BACKGROUND)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(LAYER_TYPE::LAYER_PLAYER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(LAYER_TYPE::LAYER_MONSTER)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Effect(LAYER_TYPE::LAYER_EFFECT)))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(LAYER_TYPE::LAYER_UI)))
		return E_FAIL;


	Ready_Spawners();

	Wait_ClientLevelState(LEVELSTATE::INITEND);



	Broadcast_LevelState(LEVELSTATE::INITEND);


	Start_Collision();

	return S_OK;
}

HRESULT CLevel_Konoha_Server::Tick(_float fTimeDelta)
{
	Check_Spawners();
	return S_OK;
}

HRESULT CLevel_Konoha_Server::LateTick(_float fTimeDelta)
{
	m_fBroadcastTime += fTimeDelta;
	if (m_fBroadcastTime >= 0.05f)
	{
		Broadcast_PlayerInfo();
		m_fBroadcastTime = 0.0f;
	}

	return S_OK;
}

HRESULT CLevel_Konoha_Server::Exit()
{
	End_Collision();
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Lights()
{
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	if (FAILED(Broadcast_Character()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	/*Vec3 vPos(-20.0f, 0.0f, 5.0f);

	for (_uint i = 0; i < 5; ++i)
	{
		if (FAILED(Broadcast_Monster(L"WhiteZetsu", vPos)))
			return E_FAIL;


		vPos.x += 0.01f;
	}*/
	
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_Konoha_Server::Broadcast_Character()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Vec3 vPos(-110.f, 26.f ,86.f);

	set<GameSessionRef>& Sessions = CGameSessionManager::GetInstance()->Get_Sessions();

	_uint iPlayerCount = Sessions.size();

	vPos.x -= (iPlayerCount - 1) * 3.0f * 0.5f;

	for (auto& OwnerSession : Sessions)
	{
		wstring strCharacter = OwnerSession->Get_CharacterName();
		Protocol::S_CREATE_OBJCECT pkt;
		pkt.set_strname(CAsUtils::ToString(strCharacter));
		pkt.set_iobjectid(g_iObjectID++);
		pkt.set_ilevel((uint32)LEVELID::LEVEL_KONOHA);
		pkt.set_ilayer((uint32)LAYER_TYPE::LAYER_PLAYER);
		pkt.set_iobjecttype((uint32)OBJ_TYPE::PLAYER);

		auto vPacketPos = pkt.mutable_vpos();
		vPacketPos->Resize(3, 0.0f);
		memcpy(vPacketPos->mutable_data(), &vPos, sizeof(Vec3));

		for (auto& Session : Sessions)
		{
			if (Session->GetSessionID() == OwnerSession->GetSessionID())
				pkt.set_bcontroll(true);
			else
				pkt.set_bcontroll(false);

			SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
			Session->Send(pSendBuffer);
		}



		CPlayer_Server::MODELDESC Desc;
		Desc.strFileName = CAsUtils::ToWString(pkt.strname());
		Desc.iObjectID = pkt.iobjectid();
		Desc.iLayer = pkt.ilayer();
		Desc.pGameSession = OwnerSession.get();

		CPlayer_Server* pPlayer = dynamic_cast<CPlayer_Server*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), TEXT("Prototype_GameObject_Player"), &Desc));
		if (nullptr == pPlayer)
			return E_FAIL;
		pPlayer->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPos);

		vPos.x += 3.0f;
		vPos.z -= 3.0f;
	}

	Safe_Release(pGameInstance);
}

void CLevel_Konoha_Server::Broadcast_LevelState(LEVELSTATE eState)
{
	Protocol::S_LEVEL_STATE pkt;
	pkt.set_ilevelstate((uint32)eState);

	SendBufferRef sendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(sendBuffer);
}

void CLevel_Konoha_Server::Wait_ClientLevelState(LEVELSTATE eState)
{
	set<GameSessionRef>& Sessions = CGameSessionManager::GetInstance()->Get_Sessions();

	while (true)
	{
		_uint iReadyCount = 0;

		for (auto& Session : Sessions)
		{
			if (Session->Get_LevelState() == eState)
				++iReadyCount;
		}

		if (iReadyCount == CGameSessionManager::GetInstance()->Get_SessionCount())
			break;
	}
}

HRESULT CLevel_Konoha_Server::Broadcast_PlayerInfo()
{
	auto& ObjectList = CGameInstance::GetInstance()->Find_GameObjects(LEVELID::LEVEL_KONOHA, (_uint)LAYER_TYPE::LAYER_PLAYER);

	if (ObjectList.size() == 0)
		return S_OK;

	Protocol::S_OBJECTINFO pkt;

	for (auto& Object : ObjectList)
	{
		if (Object == nullptr)
			continue;

		auto tObject = pkt.add_tobject();
		tObject->set_iobjectid(Object->Get_ObjectID());
		tObject->set_ilevel(LEVELID::LEVEL_KONOHA);
		tObject->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);


		auto vTargetPos = tObject->mutable_vtargetpos();
		vTargetPos->Resize(3, 0.0f);
		Vec3 vPlayerTargetPos = Object->Get_TargetPos();
		memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


		auto matWorld = tObject->mutable_matworld();
		matWorld->Resize(16, 0.0f);
		Matrix matPlayerWorld = Object->Get_TransformCom()->Get_WorldMatrix();
		memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));

		/*if (pPlayer->Get_ObjectTag() == L"Naruto")
			cout << matPlayerWorld.m[3][0] << " " << matPlayerWorld.m[3][1] << " " << matPlayerWorld.m[3][2] << endl;*/
	}


	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);


	return S_OK;
}

HRESULT CLevel_Konoha_Server::Broadcast_Boss(const wstring& szName, Vec3 vPos)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		Protocol::S_CREATE_OBJCECT pkt;
		pkt.set_strname(CAsUtils::ToString(szName));
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

		wstring szMonsterName = L"Prototype_GameObject_Boss_" + szName;
		CBoss_Server* pBoss = dynamic_cast<CBoss_Server*>(pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), szMonsterName, &Desc));
		if (pBoss == nullptr)
			return E_FAIL;

		pBoss->Get_TransformCom()->Set_State(CTransform::STATE::STATE_POSITION, vPos);
		pBoss->Set_Skill(nullptr);
	}
	
	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Konoha_Server::Ready_Events()
{
	CEventMgr::GetInstance()->Add_Event(new CServerEvent_MadaraMeteor((_uint)EVENT::MADARAMETEOR, nullptr, nullptr));

	return S_OK;
}

void CLevel_Konoha_Server::Ready_Spawners()
{	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(L"../Bin/Resources/SpawnerData/KonoSpawner.data", FileMode::Read);

	_uint iSpawnerCount = file->Read<_uint>();

	//Prototype_GameObject_MonsterSpawner
	
	for (_uint i = 0; i < iSpawnerCount; ++i)
	{
		_uint iObjecID = g_iObjectID++;
		CMonsterSpawner_Server* pSpawner = dynamic_cast<CMonsterSpawner_Server*>(pGameInstance->Add_GameObject((_uint)LEVEL_KONOHA, (_uint)LAYER_TYPE::LAYER_BACKGROUND, L"Prototype_GameObject_MonsterSpawner", &iObjecID));

		Vec3 vPos = file->Read<Vec3>();
		_float fRadius = file->Read<_float>();
		pSpawner->Set_Spawner(vPos, fRadius);

		_uint iMonsterCount = file->Read<_uint>();


		for (_uint i = 0; i < iMonsterCount; ++i)
		{
			pSpawner->Add_MonsterPos(file->Read<Vec3>());
		}


		Protocol::S_CREATE_OBJCECT pkt;
		pkt.set_strname("MonsterSpawner");
		pkt.set_iobjectid(iObjecID);
		pkt.set_ilevel((uint32)LEVELID::LEVEL_KONOHA);
		pkt.set_ilayer((uint32)LAYER_TYPE::LAYER_BACKGROUND);
		pkt.set_iobjecttype((uint32)OBJ_TYPE::SPAWNER);

		auto vPacketPos = pkt.mutable_vpos();
		vPacketPos->Resize(3, 0.0f);
		memcpy(vPacketPos->mutable_data(), &vPos, sizeof(Vec3));

		/*auto tMonster = pkt.add_tmonsterinfo();
		tMonster->set_ffollowdistance(10.0f);*/

		SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
		CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);

		
		m_Spawners.push_back(pSpawner);
	}



	Safe_Release(pGameInstance);
}


void CLevel_Konoha_Server::Set_CheckGruop()
{
	CCollisionManager::GetInstance()->CheckGroup((_uint)LAYER_COLLIDER::LAYER_BODY, (_uint)LAYER_COLLIDER::LAYER_BODY);
	CCollisionManager::GetInstance()->CheckGroup((_uint)LAYER_COLLIDER::LAYER_BODY, (_uint)LAYER_COLLIDER::LAYER_ATTACK);
}

void CLevel_Konoha_Server::Start_Collision()
{
	Set_CheckGruop();

	m_pCollisionThread = new thread([=]()
		{
			ThreadManager::GetInstance()->InitTLS();

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CCollisionManager* pCollisionManager = CCollisionManager::GetInstance();
			pCollisionManager->AddRef();

			if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Collision_Konoha"))))
				return FALSE;

			if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Collision_60_Konoha"))))
				return FALSE;

			_float		fTimeAcc = 0.f;


			while (!pCollisionManager->Is_Stop())
			{
				fTimeAcc += pGameInstance->Compute_TimeDelta(TEXT("Timer_Collision_Konoha"));

				if (fTimeAcc >= 1.f / 60.0f)
				{
					pCollisionManager->LateTick_Collision(pGameInstance->Compute_TimeDelta(TEXT("Timer_Collision_60_Konoha")));
					fTimeAcc = 0.f;
				}
			}

			Safe_Release(pCollisionManager);

			Safe_Release(pGameInstance);

			ThreadManager::GetInstance()->DestroyTLS();
		});
}

void CLevel_Konoha_Server::End_Collision()
{
	CCollisionManager::GetInstance()->Set_Stop(true);
	m_pCollisionThread->join();
	CCollisionManager::GetInstance()->Reset();
	Safe_Delete(m_pCollisionThread);
}


CLevel_Konoha_Server* CLevel_Konoha_Server::Create()
{
	CLevel_Konoha_Server* pInstance = new CLevel_Konoha_Server();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Konoha");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Konoha_Server::Free()
{
	__super::Free();
}

void CLevel_Konoha_Server::Check_Spawners()
{
	if (m_bBoss == true)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*for (auto& Spawners : m_Spawners)
	{
		if (Spawners->Is_Active())
			return;
	}


	list<CGameObject*> MonsterList = pGameInstance->Find_GameObjects((_uint)LEVELID::LEVEL_ARENA, (_uint)LAYER_TYPE::LAYER_MONSTER);

	for (auto& Monster : MonsterList)
	{
		if (Monster->Is_Active())
			return;
	}*/

	Make_BossSpawner();
	m_bBoss = true;
	
	
	Safe_Release(pGameInstance);
}

void CLevel_Konoha_Server::Make_BossSpawner()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Vec3 vPos = Vec3(78.43f, 27.93f, -37.87f);

	_uint iObjecID = g_iObjectID++;
	CBossSpawner_Server* pSpawner = dynamic_cast<CBossSpawner_Server*>(pGameInstance->Add_GameObject((_uint)LEVEL_KONOHA, (_uint)LAYER_TYPE::LAYER_BACKGROUND, L"Prototype_GameObject_BossSpawner", &iObjecID));

	pSpawner->Set_Spawner(vPos, 2.5f);



	Protocol::S_CREATE_OBJCECT pkt;
	pkt.set_strname("BossSpawner");
	pkt.set_iobjectid(iObjecID);
	pkt.set_ilevel((uint32)LEVELID::LEVEL_KONOHA);
	pkt.set_ilayer((uint32)LAYER_TYPE::LAYER_BACKGROUND);
	pkt.set_iobjecttype((uint32)OBJ_TYPE::SPAWNER);

	auto vPacketPos = pkt.mutable_vpos();
	vPacketPos->Resize(3, 0.0f);
	memcpy(vPacketPos->mutable_data(), &vPos, sizeof(Vec3));

	/*auto tMonster = pkt.add_tmonsterinfo();
	tMonster->set_ffollowdistance(10.0f);*/

	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);


	Safe_Release(pGameInstance);
}
