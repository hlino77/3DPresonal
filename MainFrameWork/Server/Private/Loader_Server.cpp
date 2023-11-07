#include "stdafx.h"
#include "Loader_Server.h"
#include "GameInstance.h"
#include "GameSessionManager.h"
#include "Player_Server.h"
#include "Monster_Server.h"
#include "Monster_WhiteZetsu_Server.h"
#include "Boss_Deidara_Server.h"
#include "Monster_C2Dragon_Server.h"
#include "Skill_Hiryu_Server.h"
#include "Skill_TwinBird_Server.h"

CLoader_Server::CLoader_Server()
{
}

_uint APIENTRY ThreadEntry(void* pArg)
{

	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader_Server* pLoader = (CLoader_Server*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader_Server::Initialize(LEVELID eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = eNextLevel;

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_int CLoader_Server::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_READY:
		hr = Loading_For_Level_Ready();
		break;
	case LEVEL_ARENA:
		hr = Loading_For_Level_Arena();
		break;
	case LEVEL_KONOHA:
		hr = Loading_For_Level_Konoha();
		break;
	}

	if (FAILED(hr))
		return -1;

	LeaveCriticalSection(&m_Critical_Section);

	return 0;
}

HRESULT CLoader_Server::Loading_For_Level_Ready()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.GameObject */
	m_strLoading = TEXT("객체원형을 로딩 중 입니다.");

	///* For.Prototype_GameObject_BackGround */
	//if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
	//	return E_FAIL;

	Safe_Release(pGameInstance);

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Server::Loading_For_Level_Arena()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	cout << "GamePlay Loading" << endl;

	Protocol::S_LEVEL_STATE pkt;
	pkt.set_ilevelstate((uint32)LEVELSTATE::LOADING);

	SendBufferRef sendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(sendBuffer);


	Loading_Model_For_Level_Arena();


	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Server::Loading_For_Level_Konoha()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	cout << "GamePlay Loading" << endl;

	Protocol::S_LEVEL_STATE pkt;
	pkt.set_ilevelstate((uint32)LEVELSTATE::LOADING);

	SendBufferRef sendBuffer = CServerPacketHandler::MakeSendBuffer(pkt);
	CGameSessionManager::GetInstance()->Broadcast(sendBuffer);


	Loading_Model_For_Level_Arena();


	Safe_Release(pGameInstance);

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Server::Loading_Model_For_Level_Arena()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);



	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer_Server::Create(nullptr, nullptr))))
		return E_FAIL;



	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	{
		wstring strFileName = L"WhiteZetsu";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(nullptr, nullptr, strFilePath, strFileName, false, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Sasuke";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(nullptr, nullptr, strFilePath, strFileName, false, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Naruto";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, strComponentName,
			CModel::Create(nullptr, nullptr, strFilePath, strFileName, false, false, PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Deidara";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(nullptr, nullptr, strFilePath, strFileName, false, false, PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"C2Dragon";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_ARENA, strComponentName,
			CModel::Create(nullptr, nullptr, strFilePath, strFileName, false, false, PivotMatrix))))
			return E_FAIL;
	}



	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_WhiteZetsu"),
		CMonster_WhiteZetsu_Server::Create(nullptr, nullptr))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Boss_Deidara"),
		CBoss_Deidara_Server::Create(nullptr, nullptr))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_C2Dragon"),
		CMonster_C2Dragon_Server::Create(nullptr, nullptr))))
		return E_FAIL;


	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_Hiryu"),
		CSkill_Hiryu_Server::Create(nullptr, nullptr))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Skill_TwinBird"),
		CSkill_TwinBird_Server::Create(nullptr, nullptr))))
		return E_FAIL;

	Safe_Release(pGameInstance);



	return S_OK;
}

HRESULT CLoader_Server::Loading_Model_For_Level_Konoha()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	{
		wstring strFileName = L"WhiteZetsu";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_KONOHA, strComponentName,
			CModel::Create(nullptr, nullptr, strFilePath, strFileName, false, false, PivotMatrix))))
			return E_FAIL;
	}

	
	Safe_Release(pGameInstance);
}

CLoader_Server* CLoader_Server::Create(LEVELID eNextLevel)
{
	CLoader_Server* pInstance = new CLoader_Server();

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader_Server::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);
}
