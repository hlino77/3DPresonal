#pragma once

#include "Component.h"

/* 1. ȭ�鿡 �׷������ϴ� ��ü���� �׸��� ������� ��Ƽ� �����Ѵ�. */
/* 2. �����ϰ� �ִ� ��ü���� ������(��ο���)�� �����Ѵ�. */

BEGIN(Engine)

class CShader;

class ENGINE_DLL CRenderer final : public CComponent
{
public:
	enum RENDERGROUP { RENDER_PRIORITY, RENDER_NONLIGHT, RENDER_LIGHT, INSTANCE_STATIC, RENDER_NONBLEND, RENDER_BLEND, RENDER_ALPHABLEND, RENDER_EFFECT_INSTANCE, RENDER_UI, RENDER_END };

private:
	CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	CRenderer(const CRenderer& rhs) = delete;
	virtual ~CRenderer() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Add_RenderGroup(RENDERGROUP eRenderGroup, class CGameObject* pGameObject);
	HRESULT Draw();


private:
	HRESULT Render_Priority();
	HRESULT Render_NonAlphaBlend();
	HRESULT Render_StaticInstance();
	HRESULT Render_Lights();
	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT	Render_EffectInstance();
	HRESULT Render_UI();

private:
	HRESULT Render_ModelInstancing(const wstring& szModelName);
	HRESULT Render_EffectInstancing(const wstring& szModelName);

	list<class CGameObject*>			m_RenderObjects[RENDER_END];
	unordered_map<wstring, list<class CGameObject*>> m_StaticInstance;
	unordered_map<wstring, list<class CGameObject*>> m_EffectInstance;

private:
	
	CShader* m_pInstanceShader = nullptr;

	ID3D11Buffer* m_pInstanceBuffer = nullptr;


	_uint	m_iBufferSize = 0;
public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pObject, void* pArg) override;
	virtual void Free() override;

};

END