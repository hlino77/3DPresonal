#include "stdafx.h"
#include "MainApp_Server.h"
//
#include "GameInstance.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "BufferWriter.h"
#include <tchar.h>
#include "Protocol.pb.h"
#include <iostream>
#include "Level_Loading_Server.h"
#include "StateMachine.h"
#include "ColliderSphere.h"
#include "CollisionManager.h"

CMainApp_Server::CMainApp_Server()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp_Server::Initialize()
{
	CServerPacketHandler::Init();

	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"192.168.200.155", 7777),
		MakeShared<IocpCore>(),
		MakeShared<CGameSession>, // TODO : SessionManager ��
		100);

 	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpCore()->Dispatch();
				}
			});
	}

	cout << "Server Open" << endl;

	if (FAILED(m_pGameInstance->Initialize_Engine_Server(LEVEL_END, _uint(LAYER_TYPE::LAYER_END))))
		return E_FAIL;

	if (FAILED(Initiailize_Server()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	/* 1-4. ���ӳ����� ����� ����(��)�� �����Ѵ�.   */
	if (FAILED(Open_Level(LEVEL_READY)))
		return E_FAIL;


	return S_OK;
}

void CMainApp_Server::Tick(_float fTimeDelta)
{
	CGameSessionManager::GetInstance()->Tick(fTimeDelta);

	m_pGameInstance->Tick_Server(fTimeDelta);

}

HRESULT CMainApp_Server::Render()
{
	m_pGameInstance->Render_Debug();
	return S_OK;
}

HRESULT CMainApp_Server::Initiailize_Server()
{
	CCollisionManager::GetInstance()->Reserve_Manager((_uint)LAYER_COLLIDER::LAYER_END);
	return S_OK;
}

HRESULT CMainApp_Server::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(nullptr, nullptr))))
		return E_FAIL;

	/* For.Prototype_Component_State */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(nullptr, nullptr))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"),
		CSphereCollider::Create(nullptr, nullptr))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp_Server::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;


	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading_Server::Create(eLevelID))))
		return E_FAIL;

	return S_OK;
}

CMainApp_Server* CMainApp_Server::Create()
{
	CMainApp_Server* pInstance = new CMainApp_Server();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp_Server");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp_Server::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();

	GThreadManager->Join();
}
