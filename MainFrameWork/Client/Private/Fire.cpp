#include "stdafx.h"
#include "Fire.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "VIBuffer_Point.h"
#include "RigidBody.h"
#include "Pool.h"

CFire::CFire(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CFire::CFire(const CFire& rhs)
	: CEffect(rhs)
{
}

HRESULT CFire::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CFire::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Fire";

	m_bActive = false;

	m_vColor = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
	m_vBlurColor = Vec4(0.0f, 0.0f, 0.0f, 0.0f);

	m_vUVMaxCount = Vec2(8.0f, 8.0f);
	

	m_fUVTime = 0.01f;
	m_fCurrUVTime = 0.0f;

	m_vScale = Vec2(1.0f, 1.0f);

	m_fDiscardAlpha = 0.4f;

	m_fAlphaWeight = 1.0f;


	m_pRendererCom->Reserve_RenderGroup(CRenderer::RENDER_EFFECT_INSTANCE, this);

	return S_OK;
}

void CFire::Tick(_float fTimeDelta)
{
	if (m_bStart)
	{
		if (m_bEnd)
		{
			m_fAlphaWeight += 1.0f * fTimeDelta;
			if (m_fAlphaWeight >= 1.0f)
			{
				m_fAlphaWeight = 1.0f;

				Set_Active(false);
				CPool<CFire>::Return_Obj(this);
			}
		}




		Update_UV(fTimeDelta);
	}
	else
	{
		m_fStartTime -= fTimeDelta;
		if (m_fStartTime <= 0.0f)
			m_bStart = true;
	}



}

void CFire::LateTick(_float fTimeDelta)
{
	if(m_bStart)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_EFFECT_INSTANCE, this);
}

HRESULT CFire::Render()
{
	return S_OK;
}

HRESULT CFire::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
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


	if (FAILED(m_pFireMaskTexture->Set_SRV(m_pShaderCom, "g_MaskTexture2")))
		return E_FAIL;

	if (FAILED(m_pFireDisolveTexture->Set_SRV(m_pShaderCom, "g_DisolveTexture", 1)))
		return E_FAIL;



	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render_Instance(pInstanceBuffer, iSize, sizeof(VTXINSTANCE_POINTEFFECT));


	Safe_Release(pGameInstance);
	return S_OK;
}

void CFire::Add_InstanceData(vector<Vec4>& BufferData)
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

void CFire::Appear(Vec3 vPos)
{
	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	//m_vUVIndex = Vec2(rand() % 4 * 1.0f, rand() % 3 * 1.0f);

	m_vUVIndex = Vec2(0.0f, 0.0f);

	m_vColor.w = 1.0f;


	_float fScale = (rand() % 40 + 30) * 0.01f;

	m_vScale.x = fScale;
	m_vScale.y = fScale;



	m_bEnd = false;
}

void CFire::Appear(Vec3 vPos, _float fLength, _float fTime)
{
	Vec3 vEffectPos(0.0f, 0.0f, 0.0f);

	vEffectPos.x += (rand() % 100 - 50) * 1.0f;
	vEffectPos.y += (rand() % 100 - 50) * 1.0f;
	vEffectPos.z += (rand() % 100 - 50) * 1.0f;

	vEffectPos.Normalize();
	vEffectPos *= fLength;

	vEffectPos += vPos;

	m_fStartTime = fTime;

	m_bActive = true;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEffectPos);
	m_vUVIndex = Vec2(0.0f, 0.0f);
	m_vColor.w = 1.0f;

	_float fScale = (rand() % 40 + 30) * 0.01f;
	m_vScale.x = fScale;
	m_vScale.y = fScale;


	m_fAlphaWeight = 0.0f;

	m_bEnd = false;
	m_bStart = false;
}


HRESULT CFire::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Fire_Mask"),
		TEXT("Com_MaskTexture"), (CComponent**)&m_pFireMaskTexture)))
		return E_FAIL;

	/*Com_Texture*/
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireDisolve"),
			TEXT("Com_DisolveTexture"), (CComponent**)&m_pFireDisolveTexture)))
			return E_FAIL;

	return S_OK;
}

void CFire::Update_UV(_float fTimeDelta)
{
	m_fCurrUVTime += fTimeDelta;

	if (m_fCurrUVTime >= m_fUVTime)
	{
		m_vUVIndex.x += 1.0f;

		if (m_vUVIndex.x >= m_vUVMaxCount.x)
		{
			m_vUVIndex.x = 0.0f;

			m_vUVIndex.y += 1.0f;

			if (m_vUVIndex.y >= 3.0f)
				m_bEnd = true;

			if (m_vUVIndex.y >= m_vUVMaxCount.y)
				m_vUVIndex.y = 0.0f;
		}

		m_fCurrUVTime = 0.0f;
	}
}


CGameObject* CFire::Clone(void* pArg)
{
	CFire* pInstance = new CFire(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFire");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFire* CFire::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFire* pInstance = new CFire(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFire");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CFire::Free()
{
	__super::Free();
}
