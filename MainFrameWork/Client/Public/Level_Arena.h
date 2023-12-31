#pragma once

#include "Client_Defines.h"
#include "Level.h"



BEGIN(Engine)

class CRenderer;
END



BEGIN(Client)


class CLevel_Arena final : public CLevel
{
private:
	CLevel_Arena(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Arena() = default;

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
private:
	void	Send_LevelState(LEVELSTATE eState);
	void	Wait_ServerLevelState(LEVELSTATE eState);

	HRESULT Load_MapData(LEVELID eLevel, const wstring& szFullPath);
	HRESULT Load_ColMesh(LEVELID eLevel, const wstring& szFullPath);

	HRESULT Ready_Events();

	HRESULT	Ready_Renderer();


	void			Set_CheckGruop();
	void			Start_Collision();
	void			Start_Picking();
	void			Start_PhysX();
	void			End_Picking();
	void			End_Collision();


private:
	thread* m_pCollisionThread = nullptr;
	thread* m_pPickingThread = nullptr;

	CRenderer* m_pRendererCom = nullptr;
	//_bool m_bStaticShadow = false;

public:
	static class CLevel_Arena* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END