#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "StateMachine.h"
#include "ServerSessionManager.h"
#include "ColliderSphere.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"
#include "SocketUtils.h"
#include "RigidBody.h"
#include "MainLogo.h"
#include "BackGround_Loading.h"
#include "ColliderOBB.h"
#include "CollisionManager.h"
#include "NavigationMgr.h"
#include "EventMgr.h"
#include "VIBuffer_Point.h"

CMainApp::CMainApp()	
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}


HRESULT CMainApp::Initialize()
{
	/* 1. �� ������ �ʱ�ȭ�� �����Ҳ���. */
	/* 1-1. �׷�����ġ�� �ʱ�ȭ�Ѵ�. */
	/* 1-2. ������ġ�� �ʱ�ȭ�Ѵ�. */
	/* 1-3. �Է���ġ�� �ʱ�ȭ�Ѵ�. */
	GRAPHIC_DESC		GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GraphicDesc);

	GraphicDesc.eWinMode = GRAPHIC_DESC::WINMODE_WIN;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	SocketUtils::Init();

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, _uint(LAYER_TYPE::LAYER_END), GraphicDesc, &m_pDevice, &m_pContext, g_hWnd, g_hInstance)))
		return E_FAIL;

	if (FAILED(Initialize_Client()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component()))
		return E_FAIL;

	/* 1-4. ���ӳ����� ����� ����(��)�� �����Ѵ�.   */
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;


	/* 1-4-1. ���ӳ����� ����� ���� �ڿ�(�ؽ���, ��, ��ü) ���� �غ��Ѵ�.  */

	if (FAILED(Ready_Prototype_Font()))
		return E_FAIL;

	//ShowCursor(false);

	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	Set_Mouse();

	CServerSessionManager::GetInstance()->Tick(fTimeDelta);

	/* ���ӳ��� �����ϴ� ���� ��ü���� ����. */
	/* ������ ���� */
	m_pGameInstance->Tick(fTimeDelta);
	
}

HRESULT CMainApp::Render()
{
	/* ���ӳ��� �����ϴ� ���� ��ü���� ������. */
	m_pGameInstance->Clear_BackBuffer_View(Vec4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencil_View();

	m_pRenderer_Com->Draw();

	CNavigationMgr::GetInstance()->Render();

	m_pGameInstance->Render_Debug();
	/* �ʱ�ȭ�� ��鿡 ��ü���� �׸���. */
	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* �ΰ������� �Ҵ��ϰ� �;�����. �ΰ����� ���� �ε������� ���� �����Ͽ� �ε��۾��� �����Ҳ���. */
	/* �ε���ü���� eLevelID��� ���� ���� �Ҵ社�ư� �;��� ��������ü�� �ذ���?! �����Ҵ��ϰ�;��� ������ �ڿ��� �غ���϶�� */
	if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID, L"MainLogo"))))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Set_Mouse()
{
	/*POINT MousePos;

	MousePos.x = g_iWinSizeX / 2;
	MousePos.y = g_iWinSizeY / 2;

	ClientToScreen(g_hWnd, &MousePos);
	SetCursorPos(MousePos.x, MousePos.y);*/
}

HRESULT CMainApp::Initialize_Client()
{
	CCollisionManager::GetInstance()->Reserve_Manager((_uint)LAYER_COLLIDER::LAYER_END);

	CNavigationMgr::GetInstance()->Reserve_Manager(m_pDevice, m_pContext);

	CEventMgr::GetInstance()->Reserve_EventMgr(m_pDevice, m_pContext);
	
	// Manager Reserve
	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), m_pRenderer_Com = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	Safe_AddRef(m_pRenderer_Com);

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		CVIBuffer_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	/* For.Prototype_Component_Transform */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_RigidBody */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_RigidBody"),
		CRigidBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_State */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_StateMachine"),
		CStateMachine::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_SphereColider"),
		CSphereCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_OBBColider"),
		COBBCollider::Create(m_pDevice, m_pContext))))
		return E_FAIL;


  	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MainLogo"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/BackGround/MainLogo.png"))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/BackGround/BackGround_Loading.png"))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_MainLogo"),
		CMainLogo::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround_Loading"),
		CBackGround_Loading::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Point_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtx_Point_Effect_Instance.hlsl"), VTX_LINECIRCLE_INSTANCE::Elements, VTX_LINECIRCLE_INSTANCE::iNumElements))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Font()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->AddFont(L"125", L"../Bin/Resources/Fonts/125.spritefont");


	Safe_Release(pGameInstance);
	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*	pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::CMainApp::Free()
{	
	__super::Free();

	Safe_Release(m_pRenderer_Com);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);



	Safe_Release(m_pGameInstance);
	CGameInstance::Release_Engine();

	SocketUtils::Clear();
}
