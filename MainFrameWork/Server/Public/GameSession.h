#pragma once
#include "Session.h"
#include "Server_Defines.h"

BEGIN(Server)

class CGameSession : public PacketSession
{
public:
	CGameSession();
	~CGameSession() = default;

	USE_LOCK
public:

	virtual void OnConnected() override;
	virtual void OnDisconnected() override;
	virtual void OnRecvPacket(BYTE* buffer, int32 len) override;
	virtual void OnSend(int32 len) override;


	LEVELSTATE	Get_LevelState();
	void		Set_LevelState(LEVELSTATE eState);

	_uint		Get_PlayerID();
	void		Set_PlayerID(_uint iID);

	const wstring	Get_CharacterName();
	void			Set_CharacterName(const wstring& strName);

	const wstring	Get_NickName();
	void			Set_NickName(const wstring& strName);
private:
	LEVELSTATE	m_eLevelState;
	wstring		m_strCharacter;
	wstring		m_strNickName;
	_uint		m_iPlayerID;
};

END