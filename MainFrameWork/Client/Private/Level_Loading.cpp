#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_Arena.h"
#include "Level_Lobby.h"
#include "Level_Konoha.h"

CLevel_Loading::CLevel_Loading(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}


HRESULT CLevel_Loading::Initialize(LEVELID eNextLevel, const wstring& szBackGruond)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CLevel* pLevel = pGameInstance->Get_CurrLevel();
	if (pLevel)
		pLevel->Exit();


	m_eNextLevel = eNextLevel;

	m_szBackGruond = szBackGruond;


	if (szBackGruond != L"None")
	{
 		wstring szProtoName = L"Prototype_GameObject_BackGround_" + szBackGruond;
		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOADING, _uint(LAYER_TYPE::LAYER_BACKGROUND), szProtoName))
			return E_FAIL; 
	}


	pGameInstance->Set_Loading(true);
	pGameInstance->Set_LoadingNext(eNextLevel);

	/* m_eNextLevel 에 대한 로딩작업을 수행한다. */
	/* 로딩을 겁나 하고있다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, m_eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Loading::Tick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Loading::LateTick(_float fTimeDelta)
{
	wstring		strLoadingText = m_pLoader->Get_LoadingText();

	SetWindowText(g_hWnd, strLoadingText.c_str());

	if (true == m_pLoader->Get_Finished())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		CLevel* pNewLevel = nullptr;

		switch (m_eNextLevel)
		{
		case LEVEL_LOGO:
			pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_LOBBY:
			pNewLevel = CLevel_Lobby::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_ARENA:
			pNewLevel = CLevel_Arena::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_KONOHA:
			pNewLevel = CLevel_Konoha::Create(m_pDevice, m_pContext);
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

HRESULT CLevel_Loading::Render_Debug()
{
	
	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel, const wstring& szBackGruond)
{
	CLevel_Loading*	pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel, szBackGruond)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
