#include "stdafx.h"
#include "Level_Ready_Server.h"
#include "GameInstance.h"
#include "Level_Loading_Server.h"
#include "GameSessionManager.h"
#include "LobbyUser_Server.h"
#include "Server_Defines.h"
#include "AsUtils.h"

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
	//cout << m_Users.size() << endl;


	m_fUserInfoSendTime += fTimeDelta;

	if (m_fUserInfoSendTime >= 0.1f)
	{
		m_fUserInfoSendTime = 0.0f;
		Broadcast_UserInfo();
	}

	return S_OK;
}

HRESULT CLevel_Ready_Server::LateTick(_float fTimeDelta)
{


	_uint iAllReady = 0;

	for (auto& User : m_Users)
	{
		if (User->Is_Ready())
			++iAllReady;
	}

	if (!m_Users.empty() && iAllReady >= m_Users.size())
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		if (FAILED(pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading_Server::Create(LEVEL_ARENA))))
			return E_FAIL;

		Protocol::S_OPEN_LEVEL pkt;
		pkt.set_ilevelid((uint64)LEVELID::LEVEL_ARENA);

		SendBufferRef sendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
		CGameSessionManager::GetInstance()->Broadcast(sendBuffer);

		Safe_Release(pGameInstance);
	}

	return S_OK;
}

void CLevel_Ready_Server::Add_LobbyUser(const wstring& szLobbyUser)
{
	m_Users.push_back(new CLobbyUser_Server(szLobbyUser));
}

CLobbyUser_Server* CLevel_Ready_Server::Find_LobbyUser(const wstring& szNickName)
{
	if (m_Users.empty())
		return nullptr;

	for (auto& User : m_Users)
	{
		if(User->Get_NickName() == szNickName)
			return User;
	}

	return nullptr;
}

HRESULT CLevel_Ready_Server::Ready_Layer_BackGround()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Ready_Server::Broadcast_UserInfo()
{
	if (m_Users.empty())
		return S_OK;

	Protocol::S_USERINFO pkt;

	for (auto& User : m_Users)
	{
		auto tUser = pkt.add_tuser();
		tUser->set_strnickname(CAsUtils::ToString(User->Get_NickName()));
		tUser->set_strcharacter(CAsUtils::ToString(User->Get_Character()));
		tUser->set_bready(User->Is_Ready());
	}
	
	SendBufferRef pSendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(pSendBuffer);
	

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


	for (auto& User : m_Users)
	{
		Safe_Delete(User);
	}
	m_Users.clear();
}
