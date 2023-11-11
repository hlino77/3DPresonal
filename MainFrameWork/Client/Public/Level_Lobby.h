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
	virtual HRESULT Exit() override;
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
	void	Send_UserInfo();

	void	Update_PlayerInfo();
	void	Update_CharacterSelect();
	void	Make_CharacterSelect();

	void	Update_PlayerLobby();

private:
	vector<class CLobbyUser*> m_Users;
	vector<class CUI_PlayerInfo*> m_PlayerInfo;
	vector<class CUI_CharacterSelect*> m_CharacterSelect;

	_uint m_iCharacterIndex = 0;
	_uint m_iMaxCharacterIndex = 1;


	_float m_fPlayerInfoSendTime;

public:
	static class CLevel_Lobby* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END