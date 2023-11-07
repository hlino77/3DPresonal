#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "Shader.h"
#include "GameInstance.h"

CRenderer::CRenderer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Initialize_Prototype()
{
	if (m_pDevice)
	{
		m_pInstanceShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtx_Instance.hlsl"), VTX_MODEL_INSTANCE::Elements, VTX_MODEL_INSTANCE::iNumElements);


		D3D11_BUFFER_DESC			BufferDesc;

		ZeroMemory(&BufferDesc, sizeof(D3D11_BUFFER_DESC));


		// m_BufferDesc.ByteWidth = 정점하나의 크기(Byte) * 정점의 갯수;
		BufferDesc.ByteWidth = sizeof(VTXINSTANCE) * 500;
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC; /* 정적버퍼로 할당한다. (Lock, unLock 호출 불가)*/
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		BufferDesc.MiscFlags = 0;
		BufferDesc.StructureByteStride = sizeof(VTXINSTANCE);

		D3D11_SUBRESOURCE_DATA		InitialData;

		vector<Matrix> InitMatrix;

		InitMatrix.resize(500, XMMatrixIdentity());

		InitialData.pSysMem = InitMatrix.data();

		if(FAILED(m_pDevice->CreateBuffer(&BufferDesc, &InitialData, &m_pInstanceBuffer)))
			return E_FAIL;
	}
	
	return S_OK;
}

HRESULT CRenderer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eRenderGroup, CGameObject * pGameObject)
{
	if (eRenderGroup >= RENDER_END)
		return E_FAIL;

	if (eRenderGroup == RENDERGROUP::INSTANCE_STATIC)
	{
		m_StaticInstance[pGameObject->Get_ModelName()].push_back(pGameObject);
		Safe_AddRef(pGameObject);
		return S_OK;
	}
	else if (eRenderGroup == RENDERGROUP::RENDER_EFFECT_INSTANCE)
	{
		m_EffectInstance[pGameObject->Get_ModelName()].push_back(pGameObject);
		Safe_AddRef(pGameObject);
		return S_OK;
	}



	m_RenderObjects[eRenderGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return S_OK;
}

HRESULT CRenderer::Draw()
{
	Render_Priority();
	Render_NonAlphaBlend();
	Render_StaticInstance();
	Render_Lights();
	Render_Blend();
	Render_NonLight();
	Render_AlphaBlend();
	Render_EffectInstance();
	Render_UI();

	return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& iter : m_RenderObjects[RENDER_PRIORITY])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_PRIORITY].clear();

	return S_OK;
}

HRESULT CRenderer::Render_StaticInstance()
{
	for (auto& iter : m_StaticInstance)
	{
		Render_ModelInstancing(iter.first);
		iter.second.clear();
	}

	return S_OK;
}

HRESULT CRenderer::Render_NonAlphaBlend()
{
	for (auto& iter : m_RenderObjects[RENDER_NONBLEND])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_NONBLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Lights()
{
	for (auto& iter : m_RenderObjects[RENDERGROUP::RENDER_LIGHT])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_LIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
	for (auto& iter : m_RenderObjects[RENDERGROUP::RENDER_BLEND])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_BLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
	for (auto& iter : m_RenderObjects[RENDERGROUP::RENDER_NONLIGHT])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_NONLIGHT].clear();

	return S_OK;
}

HRESULT CRenderer::Render_AlphaBlend()
{
	for (auto& iter : m_RenderObjects[RENDERGROUP::RENDER_ALPHABLEND])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_ALPHABLEND].clear();

	return S_OK;
}

HRESULT CRenderer::Render_EffectInstance()
{
	for (auto& iter : m_EffectInstance)
	{
		if (!iter.second.empty())
		{
			Render_EffectInstancing(iter.first);
			iter.second.clear();
		}
	}

	return S_OK;
}

HRESULT CRenderer::Render_UI()
{
	for (auto& iter : m_RenderObjects[RENDERGROUP::RENDER_UI])
	{
		if (FAILED(iter->Render()))
			return E_FAIL;
		Safe_Release(iter);
	}
	m_RenderObjects[RENDER_UI].clear();

	return S_OK;
}

HRESULT CRenderer::Render_ModelInstancing(const wstring& szModelName)
{
	if (nullptr == m_pInstanceShader)
		return S_OK;

	if (m_StaticInstance[szModelName].empty())
		return S_OK;

	vector<Matrix> WorldMatrix;
	WorldMatrix.reserve(500);

	for (auto& Model : m_StaticInstance[szModelName])
	{
		WorldMatrix.push_back(Model->Get_TransformCom()->Get_WorldMatrix());
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, WorldMatrix.data(), sizeof(Matrix) * WorldMatrix.size());

	m_pContext->Unmap(m_pInstanceBuffer, 0);


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pInstanceShader->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;
	if (FAILED(m_pInstanceShader->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;

	CModel* pModel = m_StaticInstance[szModelName].front()->Get_ModelCom();

	_uint		iNumMeshes = pModel->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(pModel->SetUp_OnShader(m_pInstanceShader, pModel->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(pModel->Render_Instance(m_pInstanceBuffer, WorldMatrix.size(), m_pInstanceShader, i)))
			return S_OK;
	}

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CRenderer::Render_EffectInstancing(const wstring& szModelName)
{
	vector<Matrix> WorldMatrix;
	WorldMatrix.reserve(500);

	for (auto& Object : m_EffectInstance[szModelName])
	{
		Object->Add_InstanceData(WorldMatrix);
	}

	D3D11_MAPPED_SUBRESOURCE		SubResource = {};

	m_pContext->Map(m_pInstanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	memcpy(SubResource.pData, WorldMatrix.data(), sizeof(Matrix) * WorldMatrix.size());

	m_pContext->Unmap(m_pInstanceBuffer, 0);

	m_EffectInstance[szModelName].front()->Render_Instance(m_pInstanceBuffer, WorldMatrix.size());

	return S_OK;
}


CRenderer * CRenderer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRenderer*	pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CRenderer::Clone(CGameObject* pObject, void * pArg)
{
	AddRef();

	return this;
}

void CRenderer::Free()
{
	__super::Free();

}
