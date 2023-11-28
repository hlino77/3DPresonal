#include "stdafx.h"
#include "ServerEvent_KonohaStart.h"
#include "EventMgr.h"

CServerEvent_KonohaStart::CServerEvent_KonohaStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CServerEvent(iID, pDevice, pContext)
{
}

HRESULT CServerEvent_KonohaStart::Initialize()
{

	return S_OK;
}

void CServerEvent_KonohaStart::Enter_Event()
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

void CServerEvent_KonohaStart::Exit_Event()
{

}

void CServerEvent_KonohaStart::Tick(_float fTimeDelta)
{
	if (Check_ClientState(EVENTSTATE::EVENTEND))
	{
		Send_State(EVENTSTATE::SETEND);
		CEventMgr::GetInstance()->End_Event(m_iEventID);
	}
}

void CServerEvent_KonohaStart::LateTick(_float fTimeDelta)
{

}

HRESULT CServerEvent_KonohaStart::Render()
{
	return S_OK;
}

void CServerEvent_KonohaStart::Free()
{
}
