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

class CPlayer;

class CUI_Skill final : public CUI
{
public:
	typedef struct SkillUIDesc
	{
		_uint iSkillIndex;
		wstring szUIName;
		_uint iSkillTexture;
		CPlayer* pPlayer;
	}SKILLUIDESC;

protected:
	/* 원형을 생성할 때 */
	CUI_Skill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	/* 사본을 생성할 때 */
	CUI_Skill(const CGameObject& rhs); /* 복사 생성자. */

	virtual ~CUI_Skill() = default;

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
	CTexture* m_pSkill_Mask = nullptr;
	CTexture* m_pSkill_Texture = nullptr;
	CTexture* m_pSkill_ShadowMask = nullptr;

	CPlayer* m_pPlayer = nullptr;

	Vec2 m_vMaskUV;
	_float m_fMaskUVLength;

	_uint m_iSkillIndex = 0;
	_uint m_iTextureIndex = 0;
public:
	static CUI_Skill* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override; /* 사본객체를 생성할때 원본데이터로부터 복제해올 데이터외에 더 추가해 줘야할 데이터가 있다라면 받아오겠다. */
	virtual void Free() override;
};

END