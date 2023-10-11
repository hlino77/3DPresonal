#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI_PlayerInfo final : public CUI
{
public:
	struct PlayerInfoUI
	{
		wstring szNickName;
		_uint iIndex;
	};

protected:
	/* 원형을 생성할 때 */
	CUI_PlayerInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CUI_PlayerInfo(const CGameObject& rhs); /* 복사 생성자. */

	virtual ~CUI_PlayerInfo() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual void UI_AppearTick(_float fTimeDelta) override;
	virtual void UI_Tick(_float fTimeDelta) override;
	virtual void UI_DisappearTick(_float fTimeDelta) override;

	virtual void Appear() override;


	void	Set_TextureIndex(const wstring& szCharacterName);

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;

	void		Render_String();
	HRESULT		Render_CharacterTexture();
	HRESULT		Render_ReadyMark();

	wstring		m_szNickName;


	//CharacterImg

	class CTransform* m_pTextureTransform;
	class CTexture* m_pCharacterTexture;

	_float m_fTextureSizeX;
	_float m_fTextureSizeY;
	_float m_fTextureX;
	_float m_fTextureY;

	//ReadyMark

	class CTransform* m_pMarkTransform;
	class CTexture* m_pMarkTexture;

	_float m_fMarkSizeX;
	_float m_fMarkSizeY;
	_float m_fMarkX;
	_float m_fMarkY;

	_bool  m_bReady = false;

	_float m_fMoveSpeed;


	_uint m_iTextureIndex = 0;
public:
	static CUI_PlayerInfo* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END