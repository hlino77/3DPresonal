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

class CUI_SP_Base final : public CUI
{


protected:
	/* ������ ������ �� */
	CUI_SP_Base(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CUI_SP_Base(const CGameObject& rhs); /* ���� ������. */

	virtual ~CUI_SP_Base() = default;

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
private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;


	CTexture* m_pRankTexture = nullptr;
	CTransform* m_pRankTransform = nullptr;

	_float m_fMoveSpeed;


	_float m_fRankSizeX;
	_float m_fRankSizeY;
	_float m_fRankX;
	_float m_fRankY;
public:
	static CUI_SP_Base* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END