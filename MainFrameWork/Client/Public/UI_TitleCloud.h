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

class CUI_TitleCloud final : public CUI
{
public:
	struct TitleCloud
	{
		Vec3 vTargetPos;
		Vec2 vSize;
		_uint iTexture;
	};

protected:
	/* ������ ������ �� */
	CUI_TitleCloud(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CUI_TitleCloud(const CGameObject& rhs); /* ���� ������. */

	virtual ~CUI_TitleCloud() = default;

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
	


	TitleCloud	m_tCloudInfo;

	_float m_fMoveSpeed;

public:
	static CUI_TitleCloud* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END