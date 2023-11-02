#include "stdafx.h"
#include "ClientEvent_ArenaStart.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Camera_Free.h"

CClientEvent_ArenaStart::CClientEvent_ArenaStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CClientEvent(iID, pDevice, pContext)
{
}

HRESULT CClientEvent_ArenaStart::Initialize()
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


	m_vAt = Vec3(45.0f, 30.0f, -45.0f);
	m_vTargetPos[0] = Vec3(16.0f, 67.0f, 50.0f);
	m_vTargetPos[1] = Vec3(-16.0f, 56.0f, 55.0f);
	m_vTargetPos[2] = Vec3(-40.0f, 40.0f, 20.0f);

	m_fSpeed = 0.4f;

	return S_OK;
}

void CClientEvent_ArenaStart::Enter_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	

	list<CGameObject*> Players;
	while (true)
	{
		Players = pGameInstance->Find_GameObjects(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_PLAYER);

		if (Players.size() > 0)
			break;
	}

	Safe_Release(pGameInstance);

	while (true)
	{
		if (CServerSessionManager::GetInstance()->Get_Player()->Get_Camera())
		{
			CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Set_Active(false);
			break;
		}
	}

	for (auto& Player : Players)
	{
		Player->Set_Active(false);
	}


	m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_vTargetPos[0]);
	
	Send_State(EVENTSTATE::READY);
	m_iState = (_uint)EVENTSTATE::READY;

	
	m_pCamera->Set_Active(true);
	m_fDelayTime = 1.0f;
	m_bEnd = false;
	m_fLerp = 0.0f;

	m_fCameraTime = 3.0f;
}

void CClientEvent_ArenaStart::Exit_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	m_pCamera->Set_Active(false);

	Safe_Release(pGameInstance);
}

void CClientEvent_ArenaStart::Tick(_float fTimeDelta)
{
	if (m_iState != (_uint)EVENTSTATE::EVENT)
		return;


	_float fSpeed = (m_fSpeed * fTimeDelta) * (1.0f - m_fLerp);

	m_fLerp += fSpeed;

	if (m_fLerp > 1.0f)
		m_fLerp = 1.0f;


	Vec3 vPos = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);


	Vec3 vTargetPos1 = Vec3::Lerp(m_vTargetPos[0], m_vTargetPos[1], m_fLerp);
	Vec3 vTargetPos2 = Vec3::Lerp(m_vTargetPos[1], m_vTargetPos[2], m_fLerp);
	Vec3 vResultPos = Vec3::Lerp(vTargetPos1, vTargetPos2, m_fLerp);


	m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vResultPos);
	m_pCamera->Get_TransformCom()->LookAt(m_vAt);
	

	if (m_bEnd)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime <= 0.0f)
		{
			m_iState = (_uint)EVENTSTATE::EVENTEND;
			Send_State(EVENTSTATE::EVENTEND);
		}

		return;
	}

	m_fCameraTime -= fTimeDelta;

	if (m_fCameraTime <= 0.0f)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);

		list<CGameObject*>& Players = CGameInstance::GetInstance()->Find_GameObjects(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_PLAYER);

		for (auto& Player : Players)
		{
			Player->Set_Active(true);
		}

		Safe_Release(pGameInstance);

		m_bEnd = true;
	}


}

void CClientEvent_ArenaStart::LateTick(_float fTimeDelta)
{

}


HRESULT CClientEvent_ArenaStart::Render()
{
	return S_OK;
}

void CClientEvent_ArenaStart::Free()
{
	__super::Free();
}
