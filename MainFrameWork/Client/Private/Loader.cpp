#include "stdafx.h"
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "BackGround.h"
#include "Player_Naruto.h"
#include "Camera_Free.h"
#include "StaticModel.h"
#include "ServerSession.h"
#include "Service.h"
#include "ThreadManager.h"
#include "ServerSessionManager.h"
#include "Camera_Player.h"
#include "Player_Sasuke.h"
#include "Monster_WhiteZetsu.h"

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

// unsigned(__stdcall* _beginthreadex_proc_type)(void*);

_uint APIENTRY ThreadEntry(void* pArg)
{
	
	CoInitializeEx(nullptr, 0);

	/* 새롭게 생성된 스레드가 일한다. */
	CLoader*		pLoader = (CLoader*)pArg;

	pLoader->Loading();

	return 0;
}

HRESULT CLoader::Initialize(LEVELID eNextLevel)
{
	InitializeCriticalSection(&m_Critical_Section);

	m_eNextLevel = eNextLevel;

	/* 새로운 스레드를 만들자. */
	/* 스레드 : 내 코드를 읽어주는 하나의 흐름? */
	/* 3 : 생성한 스레드가 호출해야하는 함수의 주소 */
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntry, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

_int CLoader::Loading()
{
	EnterCriticalSection(&m_Critical_Section);

	HRESULT		hr = 0;

	switch (m_eNextLevel)
	{
	case LEVEL_LOGO:
		hr = Loading_For_Level_Logo();
		break;
	case LEVEL_GAMEPLAY:
		hr = Loading_For_Level_GamePlay();
		break;
	}

	if (FAILED(hr))
		return -1;	

	LeaveCriticalSection(&m_Critical_Section);

	return 0;	
}

HRESULT CLoader::Loading_For_Level_Logo()
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

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

HRESULT CLoader::Loading_For_Level_GamePlay()
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Texture */
	m_strLoading = TEXT("텍스쳐를 로딩 중 입니다.");
	

	/* For.Mesh */
	m_strLoading = TEXT("메시를 로딩 중 입니다.");
	

	/* For.Shader */
	m_strLoading = TEXT("셰이더를 로딩 중 입니다.");

	m_strLoading = TEXT("객체 원형을 로딩 중 입니다.");

	/* For.Prototype_GameObject_Camera_Free */
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Free"),
		CCamera_Free::Create(m_pDevice, m_pContext, L"Free_Camera"))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Player"),
		CCamera_Player::Create(m_pDevice, m_pContext, L"Player_Camera"))))
		return E_FAIL;
	
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Naruto"),
		CPlayer_Naruto::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player_Sasuke"),
		CPlayer_Sasuke::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster_WhiteZetsu"),
		CMonster_WhiteZetsu::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_StaticModel"),
		CStaticModel::Create(m_pDevice, m_pContext, PROP))))
		return E_FAIL;


	m_strLoading = TEXT("모델을 로딩 중 입니다.");
	Loading_Model_For_Level_GamePlay();


	/* For.Prototype_Component_Shader_Model */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	m_strLoading = TEXT("로딩 끝.");
	m_isFinished = true;

	Safe_Release(pGameInstance);

	return S_OK;
}


HRESULT CLoader::Loading_Model_For_Level_GamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	{
		wstring strFileName = L"Naruto";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true ,PivotMatrix))))
			return E_FAIL;
	}

	{
		wstring strFileName = L"Sasuke";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true , PivotMatrix))))
			return E_FAIL;
	}


	{
		wstring strFileName = L"WhiteZetsu";
		wstring strFilePath = L"../Bin/Resources/Meshes/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true, PivotMatrix))))
			return E_FAIL;
	}

	
	{
		wstring strFileName = L"SM_ENV_TCHEXA_ArenaGround_Aa";
		wstring strFilePath = L"../Bin/Resources/Meshes/Static/";
		wstring strComponentName = L"Prototype_Component_Model_" + strFileName;

		if (FAILED(pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, strComponentName,
			CModel::Create(m_pDevice, m_pContext, strFilePath, strFileName, true , PivotMatrix))))
			return E_FAIL;
	}



	Safe_Release(pGameInstance);

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevel)
{
	CLoader*	pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevel)))
	{
		MSG_BOX("Failed to Created : CLoader");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);	

	DeleteCriticalSection(&m_Critical_Section);

	CloseHandle(m_hThread);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}