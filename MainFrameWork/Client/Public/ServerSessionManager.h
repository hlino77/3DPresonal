#pragma once

class CServerSession;

using ServerSessionRef = shared_ptr<CServerSession>;

BEGIN(Client)

class CServerSessionManager : public CBase
{
	DECLARE_SINGLETON(CServerSessionManager)

public:
	typedef struct CLIENTTIME
	{
		_uint iMinute = 0;
		_float fSecond = 0.0f;
	}TIME;

private:
	CServerSessionManager();
	virtual ~CServerSessionManager() = default;

public:
	void Tick(_float fTimeDelta);

public:
	void				Set_ServerSession(ServerSessionRef session);
	ServerSessionRef&	Get_ServerSession() { return m_pServerSession; }

	void				Send(SendBufferRef sendBuffer);
	bool				Is_Connected() { return m_bConnected; }

	void				Set_Player(class CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	class CPlayer*		Get_Player() { return m_pPlayer; }

	TIME				Get_ClientTime() { return m_tClientTime; }
	void				Set_ClientTime(TIME tTime) { m_tClientTime = tTime; }

	void				Set_NetworkDelay(_float fDelay) { m_fNetworkDelay = fDelay; }
	_float				Get_NetworkDelay() { return m_fNetworkDelay; }
private:
	ServerSessionRef m_pServerSession;

private:
	void	Send_TimeSync();


private:
	bool m_bConnected = false;
	_int m_iGameID = -1;
	class CPlayer* m_pPlayer = nullptr;

	TIME m_tClientTime;

	_float m_fNetworkDelay = 0.0f;
	_float m_fTimesync;

public:
	virtual void Free() override;
};

END