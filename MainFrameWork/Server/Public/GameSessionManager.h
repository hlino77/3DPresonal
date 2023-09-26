#pragma once

class CGameSession;

using GameSessionRef = shared_ptr<CGameSession>;

class CGameSessionManager
{
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
	void Broadcast_Others(SendBufferRef sendBuffer, uint32 _ID);
	uint32 Get_SessionCount();

	Set<GameSessionRef>& Get_Sessions() { return _sessions; }
private:
	USE_LOCK;
	Set<GameSessionRef> _sessions;

private:
	uint32 m_iSessionID = 0;
};

extern CGameSessionManager GSessionManager;