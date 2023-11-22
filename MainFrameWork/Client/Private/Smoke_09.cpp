#include "stdafx.h"
#include "Smoke_09.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"

CSmoke_09::CSmoke_09(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CSmoke_09::CSmoke_09(const CSmoke_09& rhs)
	: CEffect(rhs)
{
}

HRESULT CSmoke_09::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CSmoke_09::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Smoke_09";

	m_bActive = false;

	m_vColor = Vec4(0.7f, 0.7f, 0.7f, 1.0f);
	m_vBlurColor = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

	m_vUVMaxCount = Vec2(4.0f, 8.0f);
	

	m_fUVTime = 0.06f;
	m_fCurrUVTime = 0.0f;

	m_vScale = Vec2(5.0f, 5.0f);


	m_fDiscardAlpha = 0.7f;

	m_fAlphaWeight = 2.0f;

	return S_OK;
}

void CSmoke_09::Tick(_float fTimeDelta)
{
	if (m_bEnd)
	{
		Set_Active(false);
		CPool<CSmoke_09>::Return_Obj(this);
	}

	Update_UV(fTimeDelta);
}

void CSmoke_09::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT_INSTANCE, this);
}

HRESULT CSmoke_09::Render()
{
	return S_OK;
}

HRESULT CSmoke_09::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &Vec3(pGameInstance->Get_CamPosition()), sizeof(Vec3))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fDiscardAlpha", &m_fDiscardAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaWeight", &m_fAlphaWeight, sizeof(_float))))
		return E_FAIL;


	m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture");

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render_Instance(pInstanceBuffer, iSize, sizeof(VTXINSTANCE_POINTEFFECT));


	Safe_Release(pGameInstance);
	return S_OK;
}

void CSmoke_09::Add_InstanceData(vector<Vec4>& BufferData)
{
	Vec4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec4 vScale = m_vScale;

	BufferData.push_back(vPosition);
	BufferData.push_back(Vec4(m_vUVIndex.x, m_vUVIndex.y, m_vUVMaxCount.x, m_vUVMaxCount.y));
	BufferData.push_back(vScale);
	BufferData.push_back(m_vColor);
	BufferData.push_back(m_vBlurColor);
}

void CSmoke_09::Appear(Vec3 vPos)
{
	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	

	m_vUVIndex = Vec2(rand() % 4 * 1.0f, rand() % 3 * 1.0f);

	m_bEnd = false;
}


HRESULT CSmoke_09::Ready_Components()
{
	__super::Ready_Components();

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_PointToTex_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_09"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	return S_OK;
}

void CSmoke_09::Update_UV(_float fTimeDelta)
{
	m_fCurrUVTime += fTimeDelta;

	if (m_fCurrUVTime >= m_fUVTime)
	{
		m_vUVIndex.x += 1.0f;

		if (m_vUVIndex.x >= m_vUVMaxCount.x)
		{
			m_vUVIndex.x = 0.0f;

			m_vUVIndex.y += 1.0f;
			if (m_vUVIndex.y >= m_vUVMaxCount.y)
			{
				Set_Active(false);
				CPool<CSmoke_09>::Return_Obj(this);
			}
				
		}

		m_fCurrUVTime = 0.0f;
	}
}


CGameObject* CSmoke_09::Clone(void* pArg)
{
	CSmoke_09* pInstance = new CSmoke_09(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSmoke_09");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSmoke_09* CSmoke_09::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmoke_09* pInstance = new CSmoke_09(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSmoke_09");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSmoke_09::Free()
{
	__super::Free();
}
