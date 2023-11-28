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

class CUI_HP_Boss final : public CUI
{


protected:
	/* 원형을 생성할 때 */
	CUI_HP_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CUI_HP_Boss(const CGameObject& rhs); /* 복사 생성자. */

	virtual ~CUI_HP_Boss() = default;

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

private:

	_float m_fMoveSpeed;

	Vec4 m_vHp_GaugeColor;
	Vec4 m_vHp_DamageColor;

	Matrix matHPGauge;

	CTexture* m_pHP_Gauge = nullptr;
	CTexture* m_pHP_GaugeMask = nullptr;
	CTexture* m_pHP_Damage = nullptr;

	CGameObject* m_pBoss = nullptr;

	Vec2 m_vMaskUV;
	_float m_fMaskUVLength;

	_uint m_iPrevHp;
public:
	static CUI_HP_Boss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END