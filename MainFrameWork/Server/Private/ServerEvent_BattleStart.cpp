#include "stdafx.h"
#include "ServerEvent_BattleStart.h"
#include "EventMgr.h"
#include "Level_Arena_Server.h"

CServerEvent_BattleStart::CServerEvent_BattleStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CLevel_Arena_Server* pLevel)
	: CServerEvent(iID, pDevice, pContext)
	, m_pLevel(pLevel)
{
}

HRESULT CServerEvent_BattleStart::Initialize()
{

	return S_OK;
}

void CServerEvent_BattleStart::Enter_Event()
{
	Reset_ClientEventState();

	Send_State(EVENTSTATE::READY);


	while (true)
	{
		if (Check_ClientState(EVENTSTATE::READY))
		{
			Send_State(EVENTSTATE::EVENT);
			break;
		}
			
	}

	m_fDelayTime = 1.0f;

	m_bEnd = false;

}

void CServerEvent_BattleStart::Exit_Event()
{
	m_pLevel->Set_MonsterSpawn(true);
}

void CServerEvent_BattleStart::Tick(_float fTimeDelta)
{
	if (m_bEnd == false && Check_ClientState(EVENTSTATE::EVENTEND))
	{
		Send_State(EVENTSTATE::SETEND);
		m_bEnd = true;
	}



	if (m_bEnd)
	{
		m_fDelayTime -= fTimeDelta;
		if (m_fDelayTime <= 0.0f)
		{
			CEventMgr::GetInstance()->End_Event(m_iEventID);
		}
	}
}

void CServerEvent_BattleStart::LateTick(_float fTimeDelta)
{

}

HRESULT CServerEvent_BattleStart::Render()
{
	return S_OK;
}

void CServerEvent_BattleStart::Free()
{
}
