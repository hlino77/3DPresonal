#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Lobby final : public CLevel
{
private:
	CLevel_Lobby(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Lobby() = default;

public:
	virtual HRESULT Initialize() override;
	virtual HRESULT Tick(_float fTimeDelta) override;
	virtual HRESULT LateTick(_float fTimeDelta) override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Player(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_BackGround(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Monster(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_UI(const LAYER_TYPE eLayerType);
	HRESULT Ready_Layer_Effect(const LAYER_TYPE eLayerType);
	HRESULT Ready_Player_Camera(const LAYER_TYPE eLayerType);

	HRESULT Ready_Title(const LAYER_TYPE eLayerType);

public:

	void				Add_LobbyUser(const wstring& szLobbyUser);
	class CLobbyUser*	Find_LobbyUser(const wstring& szNickName);
private:
	void	Send_LevelState(LEVELSTATE eState);
	void	Wait_ServerLevelState(LEVELSTATE eState);


	void	Update_PlayerInfo();




private:
	vector<class CLobbyUser*> m_Users;
	vector<class CUI_PlayerInfo*> m_PlayerInfo;

public:
	static class CLevel_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END