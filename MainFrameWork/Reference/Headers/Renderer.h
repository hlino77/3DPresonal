#pragma once

#include "Component.h"

/* 1. 화면에 그려져야하는 객체들을 그리는 순서대로 모아서 보관한다. */
/* 2. 보관하고 있는 객체들의 렌더콜(드로우콜)을 수행한다. */

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
	HRESULT Draw_Server();

private:
	HRESULT Render_Priority();


	HRESULT Render_NonAlphaBlend();
	HRESULT Render_StaticInstance();
	HRESULT	Render_ShadowDepth();
	HRESULT Render_Lights();
	HRESULT Render_LightAcc();
	HRESULT Render_Deferred();


	HRESULT Render_Blend();
	HRESULT Render_NonLight();
	HRESULT Render_AlphaBlend();
	HRESULT	Render_EffectInstance();

	HRESULT Render_EffectBlur();
	HRESULT Render_EffectAcc();
	HRESULT Render_Deferred_Effects();


	HRESULT Render_UI();


	HRESULT Render_Debug();
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


	//RenderTarget
	class CTarget_Manager* m_pTarget_Manager = { nullptr };
	class CLight_Manager* m_pLight_Manager = { nullptr };

	class CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	class CShader* m_pMRTShader = { nullptr };
	class CShader* m_pEffectShader = { nullptr };


	Matrix					m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;



	//Blur
	void	Ready_BlurData();

	Vec2	m_vPixelSize;
	_int	m_iKernelSize;
	vector<_float> m_BlurWeights;


public:
	static CRenderer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(CGameObject* pObject, void* pArg) override;
	virtual void Free() override;

};

END