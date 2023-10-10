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

class CUI_CharacterSelect final : public CUI
{
public:
	struct CharacterUI
	{
		wstring szCharacterName;
		_uint iTexture;
		_bool bSelected;
		_uint iIndex;
	};
protected:
	/* ������ ������ �� */
	CUI_CharacterSelect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CUI_CharacterSelect(const CGameObject& rhs); /* ���� ������. */

	virtual ~CUI_CharacterSelect() = default;

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


	void	Set_Selected(_bool bSelect);
	const wstring& Get_CharacterName() { return m_tCharacterUI.szCharacterName; }
	_uint	Get_TextureIndex() { m_tCharacterUI.iTexture; }

private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;


private:
	CharacterUI m_tCharacterUI;

	_float m_fMoveSpeed;
public:
	static CUI_CharacterSelect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END