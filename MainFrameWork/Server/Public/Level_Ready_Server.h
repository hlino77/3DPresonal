#pragma once
#include "Server_Defines.h"
#include "Level.h"

BEGIN(Server)

class CLevel_Ready_Server final : public CLevel
{
private:
	CLevel_Ready_Server();
	virtual ~CLevel_Ready_Server() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;


	void						Add_LobbyUser(const wstring& szLobbyUser);
	class CLobbyUser_Server*	Find_LobbyUser(const wstring& szNickName);


private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Broadcast_UserInfo();



	vector<class CLobbyUser_Server*> m_Users;


	_float m_fUserInfoSendTime = 0.0f;
public:
	static class CLevel_Ready_Server* Create();
	virtual void Free() override;
};

END