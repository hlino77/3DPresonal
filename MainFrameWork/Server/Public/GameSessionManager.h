#pragma once

class CGameSession;

using GameSessionRef = shared_ptr<CGameSession>;

class CGameSessionManager
{
public:
	typedef struct SERVERTIME
	{
		_uint iMinute = 0;
		_float fSecond = 0.0f;
	}TIME;


public:
	void Tick(_float fTimeDelta);
public:
	void Add(GameSessionRef session);
	void Remove(GameSessionRef session);
	void Broadcast(SendBufferRef sendBuffer);
	void Broadcast_Others(SendBufferRef sendBuffer, uint32 _ID);

public:
	TIME Get_ServerTime() { return m_tServerTime; }
	uint32 Get_SessionCount();

	Set<GameSessionRef>& Get_Sessions() { return _sessions; }

	void				 Add_SendCount() { m_iSendCount.fetch_add(1); }
	uint64				 Get_SendCount() { return m_iSendCount.load(); }
private:
	USE_LOCK;
	Set<GameSessionRef> _sessions;

private:
	uint32 m_iSessionID = 0;
	atomic<uint64>  m_iSendCount = 0;
	TIME m_tServerTime;
};

extern CGameSessionManager GSessionManager;