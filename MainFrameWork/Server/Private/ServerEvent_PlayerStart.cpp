#include "stdafx.h"
#include "ServerEvent_PlayerStart.h"
#include "EventMgr.h"

CServerEvent_PlayerStart::CServerEvent_PlayerStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CServerEvent(iID, pDevice, pContext)
{
}

HRESULT CServerEvent_PlayerStart::Initialize()
{

	return S_OK;
}

void CServerEvent_PlayerStart::Enter_Event()
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

}

void CServerEvent_PlayerStart::Exit_Event()
{

}

void CServerEvent_PlayerStart::Tick(_float fTimeDelta)
{
	if (Check_ClientState(EVENTSTATE::EVENTEND))
	{
		Send_State(EVENTSTATE::SETEND);
		CEventMgr::GetInstance()->End_Event(m_iEventID);
	}
}

void CServerEvent_PlayerStart::LateTick(_float fTimeDelta)
{

}

HRESULT CServerEvent_PlayerStart::Render()
{
	return S_OK;
}

void CServerEvent_PlayerStart::Free()
{
}
