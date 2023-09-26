#include "stdafx.h"
#include "GameSession.h"
#include "GameSessionManager.h"


CGameSession::CGameSession()
	: m_eLevelState(LEVELSTATE::STATEEND)
{
}

void CGameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<CGameSession>(shared_from_this()));
}

void CGameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<CGameSession>(shared_from_this()));
}

void CGameSession::OnRecvPacket(BYTE* buffer, int32 len)
{
	PacketSessionRef session = static_pointer_cast<CGameSession>(shared_from_this());
	PacketHeader* header = reinterpret_cast<PacketHeader*>(buffer);

	// TODO : packetId �뿪 üũ
	CServerPacketHandler::HandlePacket(session, buffer, len);
}

void CGameSession::OnSend(int32 len)
{
}

LEVELSTATE CGameSession::Get_LevelState()
{
	READ_LOCK
	return m_eLevelState;
}

void CGameSession::Set_LevelState(LEVELSTATE eState)
{
	WRITE_LOCK
	m_eLevelState = eState;
}

const wstring CGameSession::Get_CharacterName()
{
	READ_LOCK
	return m_strCharacter;
}

void CGameSession::Set_CharacterName(const wstring& strName)
{
	WRITE_LOCK
	m_strCharacter = strName;
}
