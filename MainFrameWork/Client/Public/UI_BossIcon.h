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

class CUI_BossIcon final : public CUI
{
public:
	typedef struct UI_Boss_Icon_Desc
	{
		CGameObject* pBoss;
		_uint iTextureIndex;
	}BOSSICONDESC;

protected:
	/* ������ ������ �� */
	CUI_BossIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CUI_BossIcon(const CGameObject& rhs); /* ���� ������. */

	virtual ~CUI_BossIcon() = default;

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



	_float m_fMoveSpeed;



	_uint m_iTextureIndex;
	CGameObject* m_pBoss;
public:
	static CUI_BossIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END