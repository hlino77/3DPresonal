#include "stdafx.h"
#include "Level_Loading_Server.h"
#include "Loader_Server.h"
#include "GameInstance.h"
#include "Level_Ready_Server.h"
#include "Level_GamePlay_Server.h"

CLevel_Loading_Server::CLevel_Loading_Server()
	:CLevel(nullptr, nullptr)
{
}

HRESULT CLevel_Loading_Server::Initialize(LEVELID eNextLevel)
{
	m_eNextLevel = eNextLevel;

	/* m_eNextLevel 에 대한 로딩작업을 수행한다. */
	/* 로딩을 겁나 하고있다. */
	m_pLoader = CLoader_Server::Create(m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading_Server::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Loading_Server::LateTick(_float fTimeDelta)
{
	wstring		strLoadingText = m_pLoader->Get_LoadingText();

	if (true == m_pLoader->Get_Finished())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CLevel* pNewLevel = nullptr;

		switch (m_eNextLevel)
		{
			case LEVEL_READY:
				pNewLevel = CLevel_Ready_Server::Create();
				break;
			case LEVEL_GAMEPLAY:
				pNewLevel = CLevel_GamePlay_Server::Create();
				break;
		}

		if (nullptr == pNewLevel)
			return E_FAIL;

		if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
			return E_FAIL;

		Safe_Release(pGameInstance);
	}


	return S_OK;
}

CLevel_Loading_Server* CLevel_Loading_Server::Create(LEVELID eNextLevel)
{
	CLevel_Loading_Server* pInstance = new CLevel_Loading_Server();

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading_Server::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
