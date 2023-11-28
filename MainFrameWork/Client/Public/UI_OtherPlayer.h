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

class CUI_OtherPlayer final : public CUI
{
protected:
	/* ������ ������ �� */
	CUI_OtherPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* �纻�� ������ �� */
	CUI_OtherPlayer(const CGameObject& rhs); /* ���� ������. */

	virtual ~CUI_OtherPlayer() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_MakeSRV();


	virtual void UI_AppearTick(_float fTimeDelta) override;
	virtual void UI_Tick(_float fTimeDelta) override;
	virtual void UI_DisappearTick(_float fTimeDelta) override;

	virtual void Appear() override;
private:
	virtual HRESULT Ready_Components() override;
	virtual HRESULT Bind_ShaderResources() override;


private:

	_float m_fMoveSpeed;

	Vec4 m_vHp_GaugeColor;
	Vec4 m_vHp_DamageColor;

	Matrix matHPGauge;

	CTexture* m_pHP_Gauge = nullptr;
	CTexture* m_pHP_GaugeMask = nullptr;
	CTexture* m_pHP_Damage = nullptr;
	ID3D11ShaderResourceView* m_pNickNameSRV = nullptr;


	CGameObject* m_pPlayer = nullptr;

	Vec2 m_vMaskUV;
	_float m_fMaskUVLength;


	_uint m_iPrevHp;

	CTransform* m_pNickNameTransform = nullptr;

public:
	static CUI_OtherPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* �纻��ü�� �����Ҷ� ���������ͷκ��� �����ؿ� �����Ϳܿ� �� �߰��� ����� �����Ͱ� �ִٶ�� �޾ƿ��ڴ�. */
	virtual void Free() override;
};

END