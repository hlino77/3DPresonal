#include "stdafx.h"
#include "Level_Lobby.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Player_Naruto.h"
#include "AsUtils.h"
#include "StaticModel.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Camera_Player.h"
#include "LobbyUser.h"
#include "UI_TitleCloud.h"
#include "UI_PlayerInfo.h"
#include "UI_PlayerWindowTitle.h"

CLevel_Lobby::CLevel_Lobby(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Lobby::Initialize()
{
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


	return S_OK;
}

HRESULT CLevel_Lobby::Tick(_float fTimeDelta)
{
	Update_PlayerInfo();

	return S_OK;
}

HRESULT CLevel_Lobby::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC			LightDesc;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = Vec4(15.0f, 5.0f, 15.0f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = Vec4(1.f, 0.0f, 0.f, 1.f);
	LightDesc.vAmbient = Vec4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.vPosition = Vec4(25.0f, 5.0f, 15.0f, 1.f);
	LightDesc.fRange = 10.f;
	LightDesc.vDiffuse = Vec4(0.0f, 1.f, 0.f, 1.f);
	LightDesc.vAmbient = Vec4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = Vec4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = Vec4(0.5, 0.5, 0.5, 1.f);
	LightDesc.vAmbient = Vec4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = Vec4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{


	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_BACKGROUND), TEXT("Prototype_GameObject_BackGround_Lobby")))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	if (FAILED(Ready_Title(eLayerType)))
		return E_FAIL;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_PlayerWindow")))
		return E_FAIL;

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_PlayerWindowTitle")))
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Player_Camera(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Title(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 10;
		tCloudInfo.vSize = Vec2(175.f, 125.f);
		tCloudInfo.vTargetPos = Vec3(186.f, 45.f, 0.94f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}


	{

		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 9;
		tCloudInfo.vSize = Vec2(122.f, 100.f);
		tCloudInfo.vTargetPos = Vec3(78.f, 73.f, 0.8f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}

	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 8;
		tCloudInfo.vSize = Vec2(132.f, 110.f);
		tCloudInfo.vTargetPos = Vec3(123.f, 42.f, 0.79f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}


	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 0;
		tCloudInfo.vSize = Vec2(91.0f, 90.0f);
		tCloudInfo.vTargetPos = Vec3(39.0f, 83.0f, 0.1f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}


	
	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 6;
		tCloudInfo.vSize = Vec2(106.f, 69.f);
		tCloudInfo.vTargetPos = Vec3(287.f, 26.f, 0.91f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}

	



	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 2;
		tCloudInfo.vSize = Vec2(124.0f, 81.0f);
		tCloudInfo.vTargetPos = Vec3(57.0f, 39.0f, 0.09f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}


	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 5;
		tCloudInfo.vSize = Vec2(105.f, 67.f);
		tCloudInfo.vTargetPos = Vec3(225.f, 33.f, 0.78f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}

	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 4;
		tCloudInfo.vSize = Vec2(118.f, 65.f);
		tCloudInfo.vTargetPos = Vec3(274.f, 29.f, 0.77f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}

	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 3;
		tCloudInfo.vSize = Vec2(86.f, 49.f);
		tCloudInfo.vTargetPos = Vec3(317.f, 1.f, 0.70f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}

	{
		CUI_TitleCloud::TitleCloud tCloudInfo;
		tCloudInfo.iTexture = 1;
		tCloudInfo.vSize = Vec2(151.f, 53.f);
		tCloudInfo.vTargetPos = Vec3(136.f, 21.f, 0.08f);

		if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleCloud"), &tCloudInfo))
			return E_FAIL;

	}

	




	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_Title")))
		return E_FAIL;


	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_TitleSymbol")))
		return E_FAIL;


	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Lobby::Add_LobbyUser(const wstring& szLobbyUser)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	m_Users.push_back(new CLobbyUser(szLobbyUser));


	CUI_PlayerInfo::PlayerInfoUI tUI;
	tUI.iIndex = m_Users.size() - 1;
	tUI.szNickName = szLobbyUser;


	CUI_PlayerInfo* pNewPlayerInfo = dynamic_cast<CUI_PlayerInfo*>(pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_PlayerInfo"), &tUI));

	if (pNewPlayerInfo == nullptr)
		return;

	m_PlayerInfo.push_back(pNewPlayerInfo);

	Safe_Release(pGameInstance);
}

CLobbyUser* CLevel_Lobby::Find_LobbyUser(const wstring& szNickName)
{
	if (m_Users.empty())
		return nullptr;

	for (auto& User : m_Users)
	{
		if (User->Get_NickName() == szNickName)
			return User;
	}

	return nullptr;
}

void CLevel_Lobby::Send_LevelState(LEVELSTATE eState)
{
	Protocol::S_LEVEL_STATE pkt;
	pkt.set_ilevelstate((uint32)eState);

	SendBufferRef sendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Get_ServerSession()->Send(sendBuffer);
}

void CLevel_Lobby::Wait_ServerLevelState(LEVELSTATE eState)
{
	ServerSessionRef& ServerSession = CServerSessionManager::GetInstance()->Get_ServerSession();

	while (true)
	{
		if (ServerSession->Get_LevelState() == eState)
			break;
	}
}

void CLevel_Lobby::Update_PlayerInfo()
{
	CUI_PlayerWindowTitle* pWindowTitle = dynamic_cast<CUI_PlayerWindowTitle*>(CGameInstance::GetInstance()->
		Find_GameObejct(CGameInstance::GetInstance()->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"PlayerWindowTitle"));
	
	if (pWindowTitle->Get_UIState() != CUI::UISTATE::TICK)
		return;


	for (auto& PlayerInfo : m_PlayerInfo)
	{
		if (PlayerInfo->Is_Active())
			continue;

		PlayerInfo->Appear();
	}
}

CLevel_Lobby * CLevel_Lobby::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Lobby*	pInstance = new CLevel_Lobby(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Lobby");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Lobby::Free()
{
	__super::Free();


}
