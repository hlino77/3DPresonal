#include "stdafx.h"
#include "ServerEvent_ArenaStart.h"
#include "EventMgr.h"

CServerEvent_ArenaStart::CServerEvent_ArenaStart(_uint iID, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CServerEvent(iID, pDevice, pContext)
{
}

HRESULT CServerEvent_ArenaStart::Initialize()
{

	return S_OK;
}

void CServerEvent_ArenaStart::Enter_Event()
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

void CServerEvent_ArenaStart::Exit_Event()
{

}

void CServerEvent_ArenaStart::Tick(_float fTimeDelta)
{
	if (Check_ClientState(EVENTSTATE::EVENTEND))
	{
		Send_State(EVENTSTATE::SETEND);
		CEventMgr::GetInstance()->Start_Event((_uint)EVENT::PLAYERSTART);
	}
}

void CServerEvent_ArenaStart::LateTick(_float fTimeDelta)
{

}

HRESULT CServerEvent_ArenaStart::Render()
{
	return S_OK;
}

void CServerEvent_ArenaStart::Free()
{
}
