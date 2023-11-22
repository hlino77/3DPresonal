#include "stdafx.h"
#include "Smoke_24.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"

CSmoke_24::CSmoke_24(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CSmoke_24::CSmoke_24(const CSmoke_24& rhs)
	: CEffect(rhs)
{
}

HRESULT CSmoke_24::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CSmoke_24::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Smoke_24";

	m_bActive = false;

	m_vColor = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_vBlurColor = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

	m_vUVMaxCount = Vec2(2.0f, 2.0f);
	

	m_vScale = Vec2(1.0f, 1.0f);

	m_fDiscardAlpha = 0.5f;

	m_fAlphaWeight = 1.0f;

	m_fSpeed = 1.0f;

	return S_OK;
}

void CSmoke_24::Tick(_float fTimeDelta)
{

	if (m_bStart)
	{
		Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vPos += m_vDir * m_fSpeed * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


		if (m_bEnd)
		{
			m_fAlphaWeight += 0.5f * fTimeDelta;
			if (m_fAlphaWeight >= 1.0f)
			{
				Set_Active(false);
				CPool<CSmoke_24>::Return_Obj(this);
			}

		}
		else
		{
			m_fEffectTime -= fTimeDelta;
			if (m_fEffectTime <= 0.0f)
				m_bEnd = true;

		}
	}
	else
	{
		m_fStartTime -= fTimeDelta;
		if (m_fStartTime < 0.0f)
			m_bStart = true;
	}

	

}

void CSmoke_24::LateTick(_float fTimeDelta)
{
	if(m_bStart)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT_INSTANCE, this);
}

HRESULT CSmoke_24::Render()
{
	return S_OK;
}

HRESULT CSmoke_24::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
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

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pDisolveTexture->Set_SRV(m_pShaderCom, "g_DisolveTexture", 1)))
		return E_FAIL;



	m_pShaderCom->Begin(2);

	m_pVIBufferCom->Render_Instance(pInstanceBuffer, iSize, sizeof(VTXINSTANCE_POINTEFFECT));


	Safe_Release(pGameInstance);
	return S_OK;
}

void CSmoke_24::Add_InstanceData(vector<Vec4>& BufferData)
{
	Vec4 vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Vec4 vScale = m_vScale;

	m_vBlurColor.x = m_fAlphaWeight;

	BufferData.push_back(vPosition);
	BufferData.push_back(Vec4(m_vUVIndex.x, m_vUVIndex.y, m_vUVMaxCount.x, m_vUVMaxCount.y));
	BufferData.push_back(vScale);
	BufferData.push_back(m_vColor);
	BufferData.push_back(m_vBlurColor);
}

void CSmoke_24::Appear(Vec3 vPos)
{
	m_bActive = true;


	m_vUVIndex = Vec2(rand() % 2 * 1.0f, rand() % 2 * 1.0f);

	m_vColor.w = 0.7f;

	m_fEffectTime = (rand() % 100 + 1) * 0.01f;

	m_fAlphaWeight = 0.0f;

	m_bEnd = false;


	Vec3 vDir(0.0f, 0.0f, 0.0f);

	vDir.x = (rand() % 100 - 50) * 0.01f;
	vDir.y = (rand() % 100 - 50) * 0.01f;
	vDir.z = (rand() % 100 - 50) * 0.01f;


	vDir.Normalize();

	m_vDir = vDir;

	vPos += m_vDir * ((rand() % 100) * 0.01f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);


	m_bStart = false;
	m_fStartTime = 0.05f;
}


HRESULT CSmoke_24::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Smoke_24"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/*Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireDisolve"),
		TEXT("Com_DisolveTexture"), (CComponent**)&m_pDisolveTexture)))
		return E_FAIL;


	return S_OK;
}


CGameObject* CSmoke_24::Clone(void* pArg)
{
	CSmoke_24* pInstance = new CSmoke_24(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CSmoke_24");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSmoke_24* CSmoke_24::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSmoke_24* pInstance = new CSmoke_24(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSmoke_24");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSmoke_24::Free()
{
	__super::Free();
}
