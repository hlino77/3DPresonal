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
	/* 원형을 생성할 때 */
	CUI_OtherPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CUI_OtherPlayer(const CGameObject& rhs); /* 복사 생성자. */

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
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END