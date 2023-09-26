#pragma once
#include "Session.h"
#include "Server_Defines.h"

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

	const wstring	Get_CharacterName();
	void			Set_CharacterName(const wstring& strName);
private:
	LEVELSTATE m_eLevelState;
	wstring	   m_strCharacter;
};

