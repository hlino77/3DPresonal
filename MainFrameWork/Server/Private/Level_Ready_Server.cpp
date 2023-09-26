#include "stdafx.h"
#include "Level_Ready_Server.h"
#include "GameInstance.h"
#include "Level_Loading_Server.h"
#include "GameSessionManager.h"

CLevel_Ready_Server::CLevel_Ready_Server()
	: CLevel(nullptr, nullptr)
{
}

HRESULT CLevel_Ready_Server::Initialize()
{
	if (FAILED(Ready_Layer_BackGround()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Ready_Server::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Ready_Server::LateTick(_float fTimeDelta)
{
	_uint iUserCount = GSessionManager.Get_SessionCount();

	if (iUserCount == 2)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading_Server::Create(LEVEL_GAMEPLAY))))
			return E_FAIL;

		Protocol::S_OPEN_LEVEL pkt;
		pkt.set_ilevelid((uint64)LEVELID::LEVEL_GAMEPLAY);

		SendBufferRef sendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
		GSessionManager.Broadcast(sendBuffer);

		Safe_Release(pGameInstance);
	}

	return S_OK;
}

HRESULT CLevel_Ready_Server::Ready_Layer_BackGround()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_Ready_Server* CLevel_Ready_Server::Create()
{
	CLevel_Ready_Server* pInstance = new CLevel_Ready_Server();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Logo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Ready_Server::Free()
{
	__super::Free();
}
