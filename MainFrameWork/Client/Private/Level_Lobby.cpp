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
#include "UI_CharacterSelect.h"
#include "Player_Lobby.h"

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


	CGameInstance::GetInstance()->PlayBGM(L"MatchingLobbySound.wav", CHANNELID::CHANNEL_BGM, g_fVolume * 0.2f);

	return S_OK;
}

HRESULT CLevel_Lobby::Tick(_float fTimeDelta)
{
	Update_PlayerInfo();
	Update_CharacterSelect();


	m_fPlayerInfoSendTime += fTimeDelta;

	if (m_fPlayerInfoSendTime >= 0.05f)
	{
		m_fPlayerInfoSendTime = 0.0f;
		Send_UserInfo();
	}

	return S_OK;
}

HRESULT CLevel_Lobby::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Lobby::Exit()
{
	CGameInstance::GetInstance()->Reset_Lights();
	CGameInstance::GetInstance()->StopSoundAll();
	CGameInstance::GetInstance()->PlaySoundFile(L"UI_AllReady.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);
	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Lights()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	LIGHTDESC			LightDesc;

	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = Vec4(15.0f, 5.0f, 15.0f, 1.f);
	//LightDesc.fRange = 10.f;
	//LightDesc.vDiffuse = Vec4(1.f, 0.0f, 0.f, 1.f);
	//LightDesc.vAmbient = Vec4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	//ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	//LightDesc.eType = LIGHTDESC::TYPE_POINT;
	//LightDesc.vPosition = Vec4(25.0f, 5.0f, 15.0f, 1.f);
	//LightDesc.fRange = 10.f;
	//LightDesc.vDiffuse = Vec4(0.0f, 1.f, 0.f, 1.f);
	//LightDesc.vAmbient = Vec4(0.5f, 0.5f, 0.5f, 1.f);
	//LightDesc.vSpecular = LightDesc.vDiffuse;

	//if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
	//	return E_FAIL;

	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));
	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = Vec4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = Vec4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vAmbient = Vec4(1.0f, 1.0f, 1.0f, 1.f);
	LightDesc.vSpecular = Vec4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc)))
		return E_FAIL;

	Vec3 vLook = LightDesc.vDirection;
	vLook.Normalize();
	Vec3 vPos = -vLook * 500.0f;
	Matrix matLightView = Matrix::CreateWorld(vPos, -vLook, Vec3(0.0f, 1.0f, 0.0f));
	//pGameInstance->Ready_LightMatrix(matLightView.Invert());

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{
	CCamera::CAMERADESC tCameraDesc;

	tCameraDesc.iLayer = (_uint)LAYER_TYPE::LAYER_CAMERA;
	tCameraDesc.vEye = Vec4(0.f, 0.0f, 0.0f, 1.f);
	tCameraDesc.vAt = Vec4(0.f, 0.f, 1.f, 1.f);
	tCameraDesc.fFovy = XMConvertToRadians(60.0f);
	tCameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.fFar = 1200.0f;

	CGameObject* pCamera = CGameInstance::GetInstance()->Add_GameObject((_uint)LEVELID::LEVEL_LOBBY, (_uint)LAYER_TYPE::LAYER_CAMERA, L"Prototype_GameObject_Camera_Free", &tCameraDesc);
	pCamera->Set_Active(true);

	return S_OK;
}

HRESULT CLevel_Lobby::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_PLAYER), TEXT("Prototype_GameObject_Player_Lobby")))
		return E_FAIL;

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

	if (nullptr == pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_MatchingLobbyTitle")))
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

void CLevel_Lobby::Send_UserInfo()
{
	CLobbyUser* pLobbyUser = Find_LobbyUser(CServerSessionManager::GetInstance()->Get_NickName());

	if (pLobbyUser == nullptr)
		return;

	Protocol::S_USERINFO pkt;

	auto tUser = pkt.add_tuser();

	tUser->set_bready(pLobbyUser->Is_Ready());
	tUser->set_strnickname(CAsUtils::ToString(pLobbyUser->Get_NickName()));
	tUser->set_strcharacter(CAsUtils::ToString(pLobbyUser->Get_Character()));


	SendBufferRef pSendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Send(pSendBuffer);
}

void CLevel_Lobby::Update_PlayerInfo()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CUI_PlayerWindowTitle* pWindowTitle = dynamic_cast<CUI_PlayerWindowTitle*>(pGameInstance->
		Find_GameObejct(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"PlayerWindowTitle"));
	
	if (pWindowTitle == nullptr || pWindowTitle->Get_UIState() != CUI::UISTATE::TICK)
		return;

	_uint iIndex = 0;
	for (auto& PlayerInfo : m_PlayerInfo)
	{
		if (PlayerInfo->Is_Active())
		{
			if (PlayerInfo->Get_UIState() == CUI::UISTATE::TICK)
			{
				PlayerInfo->Set_TextureIndex(m_Users[iIndex]->Get_Character());
			}
		}
		else
			PlayerInfo->Appear();

		++iIndex;
	}

	Safe_Release(pGameInstance);
}

void CLevel_Lobby::Update_CharacterSelect()
{
	CLobbyUser* pLobbyUser = Find_LobbyUser(CServerSessionManager::GetInstance()->Get_NickName());
	if (pLobbyUser != nullptr && pLobbyUser->Is_Ready())
		return;


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	wstring szUIName = L"PlayerInfo_" + CServerSessionManager::GetInstance()->Get_NickName();

	CUI_PlayerInfo* pPlayerInfo = dynamic_cast<CUI_PlayerInfo*>(pGameInstance->
		Find_GameObejct(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, szUIName));

	if (pPlayerInfo == nullptr || pPlayerInfo->Get_UIState() != CUI::UISTATE::TICK)
		return;



	if (!m_CharacterSelect.empty())
	{
		if (KEY_TAP(KEY::ENTER))
		{
			pLobbyUser->Set_Ready();

			Protocol::S_CHARACTER_NAME pkt;
			pkt.set_strname(CAsUtils::ToString(m_CharacterSelect[m_iCharacterIndex]->Get_CharacterName()));

			SendBufferRef sendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
			CServerSessionManager::GetInstance()->Get_ServerSession()->Send(sendBuffer);
		}




		if (KEY_TAP(KEY::RIGHT_ARROW))
		{
			if (m_iCharacterIndex < m_iMaxCharacterIndex)
			{
				m_CharacterSelect[m_iCharacterIndex]->Set_Selected(false);
				++m_iCharacterIndex;
				m_CharacterSelect[m_iCharacterIndex]->Set_Selected(true);
				Update_PlayerLobby();
			}

			CGameInstance::GetInstance()->PlaySoundFile(L"UI_CharacterSelect.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);
		}


		if (KEY_TAP(KEY::LEFT_ARROW))
		{
			if (m_iCharacterIndex > 0)
			{
				m_CharacterSelect[m_iCharacterIndex]->Set_Selected(false);
				--m_iCharacterIndex;
				m_CharacterSelect[m_iCharacterIndex]->Set_Selected(true);
				Update_PlayerLobby();
			}

			CGameInstance::GetInstance()->PlaySoundFile(L"UI_CharacterSelect.wav", CHANNELID::CHANNEL_UI, g_fVolume * 0.2f);
		}


		return;
	}
	else
		Make_CharacterSelect();

	

	Safe_Release(pGameInstance);
}

void CLevel_Lobby::Make_CharacterSelect()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	{
		CUI_CharacterSelect::CharacterUI tCharacterUI;

		tCharacterUI.bSelected = true;
		tCharacterUI.iIndex = 0;
		tCharacterUI.iTexture = 1;
		tCharacterUI.szCharacterName = L"Naruto";

		CUI_CharacterSelect* pUI = dynamic_cast<CUI_CharacterSelect*>(pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_CharacterSelect"), &tCharacterUI));
		if (pUI == nullptr)
			return;

		m_CharacterSelect.push_back(pUI);
	}

	{
		CUI_CharacterSelect::CharacterUI tCharacterUI;

		tCharacterUI.bSelected = false;
		tCharacterUI.iIndex = 1;
		tCharacterUI.iTexture = 3;
		tCharacterUI.szCharacterName = L"Sasuke";

		CUI_CharacterSelect* pUI = dynamic_cast<CUI_CharacterSelect*>(pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_CharacterSelect"), &tCharacterUI));
		if (pUI == nullptr)
			return;

		m_CharacterSelect.push_back(pUI);
	}


	{
		CUI_CharacterSelect::CharacterUI tCharacterUI;

		tCharacterUI.bSelected = false;
		tCharacterUI.iIndex = 2;
		tCharacterUI.iTexture = 0;
		tCharacterUI.szCharacterName = L"None1";

		CUI_CharacterSelect* pUI = dynamic_cast<CUI_CharacterSelect*>(pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_CharacterSelect"), &tCharacterUI));
		if (pUI == nullptr)
			return;

		m_CharacterSelect.push_back(pUI);
	}

	{
		CUI_CharacterSelect::CharacterUI tCharacterUI;

		tCharacterUI.bSelected = false;
		tCharacterUI.iIndex = 3;
		tCharacterUI.iTexture = 0;
		tCharacterUI.szCharacterName = L"None2";

		CUI_CharacterSelect* pUI = dynamic_cast<CUI_CharacterSelect*>(pGameInstance->Add_GameObject(LEVEL_LOBBY, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_CharacterSelect"), &tCharacterUI));
		if (pUI == nullptr)
			return;

		m_CharacterSelect.push_back(pUI);
	}

	m_iCharacterIndex = 0;
	m_iMaxCharacterIndex = 1;
	

	Update_PlayerLobby();


	Safe_Release(pGameInstance);
}

void CLevel_Lobby::Update_PlayerLobby()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer_Lobby* pPlayer = dynamic_cast<CPlayer_Lobby*>(pGameInstance->Find_GameObejct(LEVELID::LEVEL_LOBBY, (_uint)LAYER_TYPE::LAYER_PLAYER, L"Player_Lobby"));

	if (pPlayer)
		pPlayer->Set_CharacterName(m_CharacterSelect[m_iCharacterIndex]->Get_CharacterName());

	Safe_Release(pGameInstance);
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
