#pragma once

class CServerSession;

using ServerSessionRef = shared_ptr<CServerSession>;

BEGIN(Client)

class CServerSessionManager : public CBase
{
	DECLARE_SINGLETON(CServerSessionManager)

private:
	CServerSessionManager();
	virtual ~CServerSessionManager() = default;

public:
	void Set_ServerSession(ServerSessionRef session);
	void Send(SendBufferRef sendBuffer);

	bool	Is_Connected() { return m_bConnected; }

	void Set_Player(class CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	class CPlayer* Get_Player() { return m_pPlayer; }

	ServerSessionRef& Get_ServerSession() { return m_pServerSession; }
private:
	USE_LOCK;
	ServerSessionRef m_pServerSession;

private:
	bool m_bConnected = false;
	_int m_iGameID = -1;
	class CPlayer* m_pPlayer = nullptr;

public:
	virtual void Free() override;
};

END