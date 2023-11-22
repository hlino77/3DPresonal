#include "stdafx.h"
#include "ClientEvent_MadaraMeteor.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Camera_Free.h"
#include "Boss_Madara.h"

CClientEvent_MadaraMeteor::CClientEvent_MadaraMeteor(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CClientEvent(iID, pDevice, pContext)
{
}

HRESULT CClientEvent_MadaraMeteor::Initialize()
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

	m_vOffset = Vec3(0.22f, 0.579f, -1.819f);
	m_vTargetOffset = Vec3(-0.349f, 1.34f, 0.0f);
	m_vMadaraPos = Vec3(80.41f, 27.91f, -33.61f);

	return S_OK;
}

void CClientEvent_MadaraMeteor::Enter_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	m_pMadara = dynamic_cast<CBoss_Madara*>(pGameInstance->Find_GameObejct(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_BOSS, L"Madara"));
	
	m_pMadara->Set_Render(false);
	m_pMadara->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_vMadaraPos);
	m_pMadara->Get_TransformCom()->LookAt(m_vMadaraPos + Vec3(0.0f, 0.0f, -1.0f));


	Send_State(EVENTSTATE::READY);
	m_iState = (_uint)EVENTSTATE::READY;


	m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_vMadaraPos + m_vOffset);
	m_pCamera->Get_TransformCom()->LookAt(m_vMadaraPos + m_vTargetOffset);


	m_vTargetPos = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	Vec3 vLook = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);

	vLook.Normalize();
	Vec3 vPos = m_vTargetPos + vLook;
	m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vPos);


	m_bStart = false;
	m_fDelayTime = 0.5f;


	Safe_Release(pGameInstance);

}

void CClientEvent_MadaraMeteor::Exit_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	Player_Active(true);


	CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Set_Active(true);

	Safe_Release(pGameInstance);
}

void CClientEvent_MadaraMeteor::Tick(_float fTimeDelta)
{
	if (m_iState != (_uint)EVENTSTATE::EVENT)
		return;


	if (m_bStart == false)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime <= 0.0f)
		{
			Player_Active(false);
			CServerSessionManager::GetInstance()->Get_Player()->Get_Camera()->Set_Active(false);
			m_pMadara->Set_Render(true);
			m_pCamera->Set_Active(true);
			m_bStart = true;
		}
	}
	else
	{
		Vec3 vCameraPos = m_pCamera->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
		vCameraPos = Vec3::Lerp(vCameraPos, m_vTargetPos, 2.0f * fTimeDelta);
		m_pCamera->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vCameraPos);
	}


	m_pMadara->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, m_vMadaraPos);
	m_pMadara->Get_TransformCom()->LookAt(m_vMadaraPos + Vec3(0.0f, 0.0f, -1.0f));
}

void CClientEvent_MadaraMeteor::LateTick(_float fTimeDelta)
{



}


HRESULT CClientEvent_MadaraMeteor::Render()
{
	return S_OK;
}

void CClientEvent_MadaraMeteor::Player_Active(_bool bActive)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	
	list<CGameObject*> Players = pGameInstance->Find_GameObjects(pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_PLAYER);

	for (auto& Player : Players)
	{
		Player->Set_Active(bActive);
	}


	Safe_Release(pGameInstance);
}

void CClientEvent_MadaraMeteor::Free()
{
	__super::Free();
}
