#include "stdafx.h"
#include "ClientEvent_BattleStart.h"
#include "GameInstance.h"
#include "ServerSessionManager.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Camera_Free.h"
#include "UI_BattleStart.h"

CClientEvent_BattleStart::CClientEvent_BattleStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CClientEvent(iID, pDevice, pContext)
{
}

HRESULT CClientEvent_BattleStart::Initialize()
{


	return S_OK;
}

void CClientEvent_BattleStart::Enter_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	m_pBattleStart = dynamic_cast<CUI_BattleStart*>(pGameInstance->Find_GameObejct((_uint)pGameInstance->Get_CurrLevelIndex(), (_uint)LAYER_TYPE::LAYER_UI, L"BattleStart"));

	m_fDelayTime = 1.0f;


	Send_State(EVENTSTATE::READY);
	m_iState = (_uint)EVENTSTATE::READY;

	Safe_Release(pGameInstance);

}

void CClientEvent_BattleStart::Exit_Event()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pBattleStart->Disappear();

	CGameInstance::GetInstance()->PlayBGM(L"ArenaBattle.wav", CHANNELID::CHANNEL_BGM, g_fVolume * 0.07f);


	Safe_Release(pGameInstance);
}

void CClientEvent_BattleStart::Tick(_float fTimeDelta)
{
	if (m_iState != (_uint)EVENTSTATE::EVENT)
		return;


	if (m_pBattleStart->Is_Active() == false)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime <= 0.0f)
		{
			m_pBattleStart->Appear();
			m_fDelayTime = 2.0f;
		}
	}
	

	
	if (m_pBattleStart->Get_UIState() == CUI::UISTATE::TICK)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime <= 0.0f)
		{
			m_iState = (_uint)EVENTSTATE::EVENTEND;
			Send_State(EVENTSTATE::EVENTEND);
		}
	}
}

void CClientEvent_BattleStart::LateTick(_float fTimeDelta)
{

}


HRESULT CClientEvent_BattleStart::Render()
{
	return S_OK;
}

void CClientEvent_BattleStart::Free()
{
	__super::Free();
}
