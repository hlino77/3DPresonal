#include "stdafx.h"
#include "Level_Konoha.h"
#include "GameInstance.h"
#include "Camera.h"
#include "Player_Naruto.h"
#include "AsUtils.h"
#include "StaticModel.h"
#include "ServerSessionManager.h"
#include "ServerSession.h"
#include "Camera_Player.h"
#include "AsFileUtils.h"
#include "AsUtils.h"
#include "ColMesh.h"
#include "ColliderOBB.h"
#include "ThreadManager.h"
#include "CollisionManager.h"
#include "PickingMgr.h"
#include "NavigationMgr.h"
#include "EventMgr.h"
#include "RigidBody.h"
#include "PhysXMgr.h"
#include "Pool.h"
#include "LineCircle.h"
#include "ClientEvent_MadaraMeteor.h"
#include "Renderer.h"
#include "ClientEvent_KonohaStart.h"

CLevel_Konoha::CLevel_Konoha(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Konoha::Initialize()
{
	Ready_Renderer();

	CNavigationMgr::GetInstance()->Add_Navigation(L"Kono.navi");

	Ready_Events();

	Send_LevelState(LEVELSTATE::INITREADY);
	Wait_ServerLevelState(LEVELSTATE::INITSTART);

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

	while (true)
	{
		CPlayer* pPlayer = CServerSessionManager::GetInstance()->Get_Player();
		if (pPlayer)
			break;
	}
	


	Send_LevelState(LEVELSTATE::INITEND);

	if (FAILED(Ready_Player_Camera(LAYER_TYPE::LAYER_CAMERA)))
		return E_FAIL;

	Wait_ServerLevelState(LEVELSTATE::INITEND);

	



	Start_Collision();
	Start_Picking();
	Start_PhysX();

	return S_OK;
}

HRESULT CLevel_Konoha::Tick(_float fTimeDelta)
{
	/*if (KEY_TAP(KEY::F9))
		m_pRendererCom->Set_StaticShadow();*/


	return S_OK;
}

HRESULT CLevel_Konoha::LateTick(_float fTimeDelta)
{
	return S_OK;
}

HRESULT CLevel_Konoha::Exit()
{
	End_Collision();
	End_Picking();
	CPhysXMgr::GetInstance()->Reset();
	CGameInstance::GetInstance()->Reset_Lights();
	CGameInstance::GetInstance()->Reset_QaudTree();
	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Lights()
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
	LightDesc.vDirection = Vec4(-0.079633f, -0.82389f, 0.561141f, 0.f);
	LightDesc.vDiffuse = Vec4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vAmbient = Vec4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vSpecular = Vec4(0.4f, 0.4f, 0.4f, 1.f);


	CTexture* pStaticShadowMap = CTexture::Create(m_pDevice, m_pContext, L"../Bin/Resources/Textures/LightMap/KonohaLight.dds");

	if (FAILED(pGameInstance->Add_Light(m_pDevice, m_pContext, LightDesc, pStaticShadowMap)))
		return E_FAIL;


	Vec3 vLook = LightDesc.vDirection;
	vLook.Normalize();
	//Vec3 vPos = Vec3(-80.78f, 83.75f, 33.80f);
	//Matrix matLightView = Matrix::CreateWorld(vPos, -vLook, Vec3(0.0f, 1.0f, 0.0f));

	Vec3 vOffset = Vec3(37.75f, 188.02f, -123.82f);

	pGameInstance->Ready_StaticLightMatrix(vOffset, vLook);
	vOffset.Normalize();
	vOffset *= 30.0f;
	pGameInstance->Ready_LightMatrix(vOffset, vLook);


	//Matrix matLightView = Matrix::CreateWorld(vPos, -vLook, Vec3(0.0f, 1.0f, 0.0f));
	//pGameInstance->Ready_LightMatrix(matLightView.Invert());

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_Camera(const LAYER_TYPE eLayerType)
{


	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_Player(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_BackGround(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	pGameInstance->Make_QaudTree(Vec3(0.0f, 62.2f, 0.0f), Vec3(250.0f, 70.0f, 220.0f), 3);

	Load_ColMesh(LEVELID::LEVEL_KONOHA, L"../Bin/Resources/ColMeshData/Kono.data");
	Load_MapData(LEVELID::LEVEL_KONOHA, L"../Bin/Resources/MapData/Kono.data");
	

	pGameInstance->GetInstance()->Add_GameObject(LEVELID::LEVEL_ARENA, (_uint)LAYER_TYPE::LAYER_SKYBOX, L"Prototype_GameObject_SkyBoxNight");

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_Monster(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_UI(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/*for (_uint i = 0; i < 1; ++i)
	{
		if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_UI"), LEVEL_GAMEPLAY, _uint(eLayerType))))
			return E_FAIL;

	} */


	if (nullptr == pGameInstance->Add_GameObject(LEVEL_KONOHA, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_BattleStart")))
		return E_FAIL;


	if (nullptr == pGameInstance->Add_GameObject(LEVEL_KONOHA, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_SP_Base")))
		return E_FAIL;


	if (nullptr == pGameInstance->Add_GameObject(LEVEL_KONOHA, _uint(LAYER_TYPE::LAYER_UI), TEXT("Prototype_GameObject_UI_HP_Base")))
		return E_FAIL;


	Safe_Release(pGameInstance);



	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Layer_Effect(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);




	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Player_Camera(const LAYER_TYPE eLayerType)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CPlayer* pPlayer = CServerSessionManager::GetInstance()->Get_Player();

	if (pPlayer == nullptr)
		return E_FAIL;

	CCamera_Player::PlayerCameraDesc CameraDesc;

	CameraDesc.tCameraDesc.iLayer = (_uint)eLayerType;
	CameraDesc.tCameraDesc.vEye = Vec4(0.f, 10.f, -10.f, 1.f);
	CameraDesc.tCameraDesc.vAt = Vec4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.tCameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.tCameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.tCameraDesc.fNear = 0.2f;
	CameraDesc.tCameraDesc.fFar = 1200.0f;

	CameraDesc.tCameraDesc.TransformDesc.fSpeedPerSec = 5.f;
	CameraDesc.tCameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	CameraDesc.fSensitive = 0.1f;
	CameraDesc.fSpeed = 4.f;
	CameraDesc.pPlayer = pPlayer;
	CameraDesc.vInitPos = pPlayer->Get_TransformCom()->Get_State(CTransform::STATE::STATE_POSITION) + Vec3(0.0f, 5.0f, -5.0f);

	CGameObject* pCamera = pGameInstance->Add_GameObject(LEVEL_KONOHA, _uint(eLayerType), TEXT("Prototype_GameObject_Camera_Player"), &CameraDesc);
	if (pCamera == nullptr)
		return E_FAIL;

	pPlayer->Set_Camera(dynamic_cast<CCamera_Player*>(pCamera));

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Konoha::Send_LevelState(LEVELSTATE eState)
{
	Protocol::S_LEVEL_STATE pkt;
	pkt.set_ilevelstate((uint32)eState);

	SendBufferRef sendBuffer = CClientPacketHandler::MakeSendBuffer(pkt);
	CServerSessionManager::GetInstance()->Get_ServerSession()->Send(sendBuffer);
}

void CLevel_Konoha::Wait_ServerLevelState(LEVELSTATE eState)
{
	ServerSessionRef& ServerSession = CServerSessionManager::GetInstance()->Get_ServerSession();

	while (true)
	{
		if (ServerSession->Get_LevelState() == eState)
			break;
	}
}

HRESULT CLevel_Konoha::Load_MapData(LEVELID eLevel, const wstring& szFullPath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(szFullPath, FileMode::Read);

	Matrix		PivotMatrix = XMMatrixIdentity();
	PivotMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));


	_uint iSize = file->Read<_uint>();

	for (_uint i = 0; i < iSize; ++i)
	{
		wstring szModelName = CAsUtils::ToWString(file->Read<string>());
		Matrix	matWorld = file->Read<Matrix>();


		{
			CStaticModel::MODELDESC Desc;
			Desc.strFileName = szModelName;
			Desc.iLayer = (_uint)LAYER_TYPE::LAYER_BACKGROUND;


			CGameObject* pObject = pGameInstance->Add_GameObject(eLevel, Desc.iLayer, TEXT("Prototype_GameObject_StaticModel"), &Desc);
			if (nullptr == pObject)
			{
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			pObject->Get_TransformCom()->Set_WorldMatrix(matWorld);


			_uint iColliderCount = file->Read<_uint>();
			CStaticModel* pStaticModel = dynamic_cast<CStaticModel*>(pObject);


			for (_uint i = 0; i < iColliderCount; ++i)
			{
				pStaticModel->Add_Collider();

				CSphereCollider* pCollider = pStaticModel->Get_StaticCollider(i);

				{
					Vec3 vOffset = file->Read<Vec3>();
					pCollider->Set_Offset(vOffset);


					_float fRadius = file->Read<_float>();
					pCollider->Set_Radius(fRadius);


					pCollider->Set_Center();
				}

				_bool bChild = file->Read<_bool>();

				if (bChild)
				{
					pStaticModel->Add_ChildCollider(i);

					COBBCollider* pChild = dynamic_cast<COBBCollider*>(pCollider->Get_Child());


					Vec3 vOffset = file->Read<Vec3>();
					pChild->Set_Offset(vOffset);

					Vec3 vScale = file->Read<Vec3>();
					pChild->Set_Scale(vScale);

					Quaternion vQuat = file->Read<Quaternion>();
					pChild->Set_Orientation(vQuat);

					pChild->Set_StaticBoundingBox();
				}
			}


			{
				vector<CSphereCollider*>& Colliders = pStaticModel->Get_StaticColliders();

				for (auto& Collider : Colliders)
				{
					pGameInstance->Add_Object_To_QuadTree(Collider);
				}
			}



			//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_STATICSHADOW, pObject);
		}
	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Konoha::Load_ColMesh(LEVELID eLevel, const wstring& szFullPath)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	shared_ptr<CAsFileUtils> file = make_shared<CAsFileUtils>();
	file->Open(szFullPath, FileMode::Read);


	_uint iSize = file->Read<_uint>();

	for (_uint i = 0; i < iSize; ++i)
	{
		wstring szModelName = CAsUtils::ToWString(file->Read<string>());
		Matrix	matWorld = file->Read<Matrix>();


		CColMesh::MODELDESC Desc;
		Desc.strFileName = szModelName;
		Desc.iLayer = (_uint)LAYER_TYPE::LAYER_COLMESH;


		CGameObject* pObject = pGameInstance->Add_GameObject(eLevel, Desc.iLayer, TEXT("Prototype_GameObject_ColMesh"), &Desc);
		if (nullptr == pObject)
		{
			Safe_Release(pGameInstance);
			return E_FAIL;
		}

		pObject->Get_TransformCom()->Set_WorldMatrix(matWorld);
		CPhysXMgr::GetInstance()->Register_ColMesh(pObject);


		_uint iColliderCount = file->Read<_uint>();
		CColMesh* pColMesh = dynamic_cast<CColMesh*>(pObject);


		for (_uint i = 0; i < iColliderCount; ++i)
		{
			pColMesh->Add_Collider();

			CSphereCollider* pCollider = pColMesh->Get_StaticCollider(i);

			{
				Vec3 vOffset = file->Read<Vec3>();
				pCollider->Set_Offset(vOffset);


				_float fRadius = file->Read<_float>();
				pCollider->Set_Radius(fRadius);


				pCollider->Set_Center();
			}

			_bool bChild = file->Read<_bool>();

			if (bChild)
			{
				pColMesh->Add_ChildCollider(i);

				COBBCollider* pChild = dynamic_cast<COBBCollider*>(pCollider->Get_Child());


				Vec3 vOffset = file->Read<Vec3>();
				pChild->Set_Offset(vOffset);

				Vec3 vScale = file->Read<Vec3>();
				pChild->Set_Scale(vScale);

				Quaternion vQuat = file->Read<Quaternion>();
				pChild->Set_Orientation(vQuat);

				pChild->Set_StaticBoundingBox();
			}
		}




	}

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Events()
{
	CEventMgr::GetInstance()->Add_Event(new CClientEvent_MadaraMeteor((_uint)EVENT::MADARAMETEOR, m_pDevice, m_pContext));
	CEventMgr::GetInstance()->Add_Event(new CClientEvent_KonohaStart((_uint)EVENT::KONOHASTART, m_pDevice, m_pContext));

	return S_OK;
}

HRESULT CLevel_Konoha::Ready_Renderer()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	m_pRendererCom = dynamic_cast<CRenderer*>(pGameInstance->Clone_Component(nullptr, LEVEL_STATIC, L"Prototype_Component_Renderer"));
	if (m_pRendererCom == nullptr)
		return E_FAIL;


	Safe_Release(pGameInstance);
	return S_OK;
}



void CLevel_Konoha::Set_CheckGruop()
{
	CCollisionManager::GetInstance()->CheckGroup((_uint)LAYER_COLLIDER::LAYER_BODY, (_uint)LAYER_COLLIDER::LAYER_BODY);
}

void CLevel_Konoha::Start_Collision()
{
	Set_CheckGruop();

	m_pCollisionThread = new thread([=]()
		{
			ThreadManager::GetInstance()->InitTLS();

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CCollisionManager* pCollisionManager = CCollisionManager::GetInstance();
			pCollisionManager->AddRef();

			if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Collision_Konoha"))))
				return FALSE;

			if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Collision_60_Konoha"))))
				return FALSE;

			_float		fTimeAcc = 0.f;


			while (!pCollisionManager->Is_Stop())
			{
				fTimeAcc += pGameInstance->Compute_TimeDelta(TEXT("Timer_Collision_Konoha"));

				if (fTimeAcc >= 1.f / 60.0f)
				{
					pCollisionManager->LateTick_Collision(pGameInstance->Compute_TimeDelta(TEXT("Timer_Collision_60_Konoha")));
					fTimeAcc = 0.f;
				}
			}

			Safe_Release(pCollisionManager);

			Safe_Release(pGameInstance);

			ThreadManager::GetInstance()->DestroyTLS();
		});
}

void CLevel_Konoha::Start_Picking()
{
	m_pPickingThread = new thread([=]()
		{
			ThreadManager::GetInstance()->InitTLS();

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			CPickingMgr* pPickingMgr = CPickingMgr::GetInstance();
			pPickingMgr->AddRef();

			pPickingMgr->Set_Player(CServerSessionManager::GetInstance()->Get_Player());

			if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Picking_Konoha"))))
				return FALSE;

			if (FAILED(pGameInstance->Add_Timer(TEXT("Timer_Picking_60_Konoha"))))
				return FALSE;

			_float		fTimeAcc = 0.f;


			while (!pPickingMgr->Is_Stop())
			{
				fTimeAcc += pGameInstance->Compute_TimeDelta(TEXT("Timer_Picking_Konoha"));

				if (fTimeAcc >= 1.f / 60.0f)
				{
					pPickingMgr->Update_PickingMgr();
					fTimeAcc = 0.f;
				}
			}

			Safe_Release(pPickingMgr);

			Safe_Release(pGameInstance);

			ThreadManager::GetInstance()->DestroyTLS();
		});

}

void CLevel_Konoha::Start_PhysX()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	list<CGameObject*>& PlayerList = pGameInstance->Find_GameObjects(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_PLAYER);

	for (auto& Player : PlayerList)
	{
		Player->Set_Navi(false);
		Player->Get_RigidBody()->Set_PhysX(true);
		CPhysXMgr::GetInstance()->Add_PlayObject(Player);
	}


	Safe_Release(pGameInstance);

}

void CLevel_Konoha::End_Picking()
{
	CPickingMgr::GetInstance()->Set_Stop(true);
	m_pPickingThread->join();
	CPickingMgr::GetInstance()->Reset();
	Safe_Delete(m_pPickingThread);
}

void CLevel_Konoha::End_Collision()
{
	CCollisionManager::GetInstance()->Set_Stop(true);
	m_pCollisionThread->join();
	CCollisionManager::GetInstance()->Reset();
	Safe_Delete(m_pCollisionThread);
}

CLevel_Konoha * CLevel_Konoha::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_Konoha*	pInstance = new CLevel_Konoha(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CLevel_Konoha");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Konoha::Free()
{
	__super::Free();


}
