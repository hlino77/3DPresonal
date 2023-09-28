#include "stdafx.h"
#include "Level_GamePlay_Server.h"
#include "GameSessionManager.h"
#include "GameSession.h"
#include "AsUtils.h"
#include "Player_Server.h"
#include "GameInstance.h"

CLevel_GamePlay_Server::CLevel_GamePlay_Server()
	: CLevel(nullptr, nullptr)
{
}

HRESULT CLevel_GamePlay_Server::Initialize()
{
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


	Wait_ClientLevelState(LEVELSTATE::INITEND);
	Broadcast_LevelState(LEVELSTATE::INITEND);

	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::LateTick(_float fTimeDelta)
{
	m_fBroadcastTime += fTimeDelta;
	if (m_fBroadcastTime >= 0.1f)
	{
		Broadcast_PlayerInfo();
		m_fBroadcastTime = 0.0f;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Lights()
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	if (FAILED(Broadcast_Character()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{
	return S_OK;
}

HRESULT CLevel_GamePlay_Server::Broadcast_Character()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Set<GameSessionRef>& Sessions = GSessionManager.Get_Sessions();
	for (auto& OwnerSession : Sessions)
	{
		wstring strCharacter = OwnerSession->Get_CharacterName();
		Protocol::S_CREATE_OBJCECT pkt;
		pkt.set_strname(CAsUtils::ToString(strCharacter));
		pkt.set_iobjectid(g_iObjectID++);
		pkt.set_ilevel((uint32)LEVELID::LEVEL_GAMEPLAY);
		pkt.set_ilayer((uint32)LAYER_TYPE::LAYER_PLAYER);
		pkt.set_iobjecttype((uint32)OBJ_TYPE::PLAYER);

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

		if (nullptr == pGameInstance->Add_GameObject(pkt.ilevel(), pkt.ilayer(), TEXT("Prototype_GameObject_Player"), &Desc))
			return E_FAIL;
	}

	Safe_Release(pGameInstance);
}

void CLevel_GamePlay_Server::Broadcast_LevelState(LEVELSTATE eState)
{
	Protocol::S_LEVEL_STATE pkt;
	pkt.set_ilevelstate((uint32)eState);

	SendBufferRef sendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	GSessionManager.Broadcast(sendBuffer);
}

void CLevel_GamePlay_Server::Wait_ClientLevelState(LEVELSTATE eState)
{
	Set<GameSessionRef>& Sessions = GSessionManager.Get_Sessions();

	while (true)
	{
		_uint iReadyCount = 0;

		for (auto& Session : Sessions)
		{
			if (Session->Get_LevelState() == eState)
				++iReadyCount;
		}

		if (iReadyCount == GSessionManager.Get_SessionCount())
			break;
	}
}

HRESULT CLevel_GamePlay_Server::Broadcast_PlayerInfo()
{
	auto& ObjectList = CGameInstance::GetInstance()->Find_GameObjects(LEVELID::LEVEL_GAMEPLAY, (_uint)LAYER_TYPE::LAYER_PLAYER);

	if (ObjectList.size() == 0)
		return S_OK;

	Protocol::S_PLAYERINFO pkt;

	for (auto& Player : ObjectList)
	{
		CPlayer_Server* pPlayer = dynamic_cast<CPlayer_Server*>(Player);

		if (pPlayer == nullptr)
			continue;

		auto tPlayer = pkt.add_tplayer();
		tPlayer->set_iplayerid(Player->Get_ObjectID());
		tPlayer->set_ilevel(LEVELID::LEVEL_GAMEPLAY);
		tPlayer->set_ilayer((_uint)LAYER_TYPE::LAYER_PLAYER);


		auto vTargetPos = tPlayer->mutable_vtargetpos();
		vTargetPos->Resize(3, 0.0f);
		Vec3 vPlayerTargetPos = pPlayer->Get_TargetPos();
		memcpy(vTargetPos->mutable_data(), &vPlayerTargetPos, sizeof(Vec3));


		auto matWorld = tPlayer->mutable_matworld();
		matWorld->Resize(16, 0.0f);
		Matrix matPlayerWorld = pPlayer->Get_TransformCom()->Get_WorldMatrix();
		memcpy(matWorld->mutable_data(), &matPlayerWorld, sizeof(Matrix));

		if (pPlayer->Get_ObjectTag() == L"Naruto")
			cout << vPlayerTargetPos.x << " " << vPlayerTargetPos.y << " " << vPlayerTargetPos.z << endl;
	}


	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	GSessionManager.Broadcast(pSendBuffer);


	return S_OK;
}

CLevel_GamePlay_Server* CLevel_GamePlay_Server::Create()
{
	CLevel_GamePlay_Server* pInstance = new CLevel_GamePlay_Server();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_GamePlay");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay_Server::Free()
{
	__super::Free();
}
