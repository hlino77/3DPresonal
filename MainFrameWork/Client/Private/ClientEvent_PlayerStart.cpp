#include "stdafx.h"
#include "ClientEvent_PlayerStart.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Camera_Free.h"

CClientEvent_PlayerStart::CClientEvent_PlayerStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CClientEvent(iID, pDevice, pContext)
{
}

HRESULT CClientEvent_PlayerStart::Initialize()
{
	CCamera::CAMERADESC tCameraDesc;
	
	tCameraDesc.iLayer = (_uint)LAYER_TYPE::LAYER_CAMERA;
	tCameraDesc.vEye = Vec4(0.f, 10.f, -10.f, 1.f);
	tCameraDesc.vAt = Vec4(0.f, 0.f, 0.f, 1.f);
	tCameraDesc.fFovy = XMConvertToRadians(60.0f);
	tCameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	tCameraDesc.fNear = 0.2f;
	tCameraDesc.fFar = 1000.0f;

	CGameObject* pCamera = CGameInstance::GetInstance()->Add_GameObject((_uint)LEVELID::LEVEL_ARENA, (_uint)LAYER_TYPE::LAYER_CAMERA, L"Prototype_GameObject_Camera_Free", &tCameraDesc);

	m_pCamera = dynamic_cast<CCamera_Free*>(pCamera);

	return S_OK;
}

void CClientEvent_PlayerStart::Enter_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	list<CGameObject*> Players = pGameInstance->Find_GameObjects(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_PLAYER);

	Safe_Release(pGameInstance);

	m_iPlayerCount = Players.size();
	Vec3 vPlayerPos = Players.back()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	Vec3 vStartPos = vPlayerPos + Vec3(0.0f, 1.0f, 2.0f);

	Vec3 vAt = vStartPos + Vec3(0.0f, 0.0f, -1.0f);

	
	m_vEndPos = Players.front()->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

	m_vEndPos += Vec3(0.0f, 1.0f, 2.0f);

	m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vStartPos);
	m_pCamera->Get_TransformCom()->LookAt_ForLandObject(vAt);

	Send_State(EVENTSTATE::READY);
	m_iState = (_uint)EVENTSTATE::READY;



	if (m_iPlayerCount == 1)
		m_fCameraTime = 2.0f;
	else
		m_fCameraTime = 0.5f;

	m_fCurrTime = 0.0f;

	m_pCamera->Set_Active(true);
}

void CClientEvent_PlayerStart::Exit_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Set_Active(true);

	m_pCamera->Set_Active(false);

	Safe_Release(pGameInstance);
}

void CClientEvent_PlayerStart::Tick(_float fTimeDelta)
{
	if (m_iState != (_uint)EVENTSTATE::EVENT)
		return;

	
	if (m_iPlayerCount == 1)
	{
		m_fCurrTime += fTimeDelta;
		if (m_fCurrTime >= m_fCameraTime)
		{
			m_iState = (_uint)EVENTSTATE::EVENTEND;
			Send_State(EVENTSTATE::EVENTEND);
		}
	}
	else
	{
		Vec3 vPos = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);

		if (vPos.x <= m_vEndPos.x)
		{
			vPos.x = m_vEndPos.x;
			m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);

			m_fCurrTime += fTimeDelta;
			if (m_fCurrTime >= m_fCameraTime)
			{
				m_iState = (_uint)EVENTSTATE::EVENTEND;
				Send_State(EVENTSTATE::EVENTEND);
			}
		}
		else
		{
			vPos.x -= 1.0f * fTimeDelta;
			m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);
		}
	}
		
	

}

void CClientEvent_PlayerStart::LateTick(_float fTimeDelta)
{

}


HRESULT CClientEvent_PlayerStart::Render()
{
	return S_OK;
}

void CClientEvent_PlayerStart::Free()
{
	__super::Free();
}
