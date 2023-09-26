#include "stdafx.h"
#include "ServerSessionManager.h"
#include "Client_Defines.h"
#include "ServerSession.h"

IMPLEMENT_SINGLETON(CServerSessionManager)

CServerSessionManager::CServerSessionManager()
{
}

void CServerSessionManager::Set_ServerSession(ServerSessionRef session)
{
	m_pServerSession = session;
	m_bConnected = true;
}

void CServerSessionManager::Send(SendBufferRef sendBuffer)
{
	m_pServerSession->Send(sendBuffer);
}


void CServerSessionManager::Free()
{
	__super::Free();

	m_pServerSession = nullptr;
}
