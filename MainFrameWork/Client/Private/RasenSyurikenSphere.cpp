#include "stdafx.h"
#include "RasenSyurikenSphere.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Pool.h"
#include "SmokeRing_Syuriken.h"


CRasenSyurikenSphere::CRasenSyurikenSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CRasenSyurikenSphere::CRasenSyurikenSphere(const CRasenSyurikenSphere& rhs)
	: CEffect(rhs)
{
}

HRESULT CRasenSyurikenSphere::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CRasenSyurikenSphere::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_RasenSyurikenSphere";

	m_bActive = false;


	m_vColor = Vec4(0.03f, 0.59f, 0.94f, 1.0f);
	m_vBlur = Vec4(0.03f, 0.69f, 0.94f, 1.0f);



	m_vTargetScale = Vec3(10.0f, 10.0f, 10.0f);


	m_fAlphaWeight = 1.0f;


	m_fSmokeDelay = 0.05f;
	

	return S_OK;
}

void CRasenSyurikenSphere::Tick(_float fTimeDelta)
{
	m_fUVX -= 2.0f * fTimeDelta;

	Vec3 vScale = m_pTransformCom->Get_Scale();
	vScale = Vec3::Lerp(vScale, m_vTargetScale, 2.0f * fTimeDelta);
	m_pTransformCom->Set_Scale(vScale);


	m_fEffectTime -= fTimeDelta;
	if (m_fEffectTime <= 0.0f)
	{
		m_bEnd = true;
	}


	if (m_bEnd)
	{
		/*m_vColor.w -= 2.0f * fTimeDelta;
		if (m_vColor.w <= 0.0f)
		{
			Set_Active(false);
			CPool<CRasenSyurikenSphere>::Return_Obj(this);
		}*/


		m_fAlphaWeight += 0.5f * fTimeDelta;
		if (m_fAlphaWeight >= 1.0f)
		{
			Set_Active(false);
			CPool<CRasenSyurikenSphere>::Return_Obj(this);
		}
	}
	else
	{
		m_fCurrTime += fTimeDelta;
		if (m_fCurrTime >= m_fSmokeDelay)
		{
			Vec3 vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			CSmokeRing_Syuriken* pSmoke = CPool<CSmokeRing_Syuriken>::Get_Obj();

			pSmoke->Appear(vPos, 20.0f, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0.8f);

			m_fCurrTime = 0.0f;
		}
	}
}

void CRasenSyurikenSphere::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CRasenSyurikenSphere::Render()
{
	
	if (nullptr == m_pModelCom || nullptr == m_pShaderCom)
		return S_OK;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_pTransformCom->Get_WorldMatrix())))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(Vec4))))
		return S_OK;

	Vec4 vBlur = m_vBlur * m_vColor.w * 0.5f;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &vBlur, sizeof(Vec4))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVX", &m_fUVX, sizeof(_float))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaWeight", &m_fAlphaWeight, sizeof(_float))))
		return S_OK;


	if (FAILED(m_pDisolveTexture->Set_SRV(m_pShaderCom, "g_DisolveTexture")))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;
	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{


		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 9)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}



void CRasenSyurikenSphere::Appear(Vec3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_bActive = true;

	m_bEnd = false;

	m_fUVX = 0.0f;

	m_pTransformCom->Set_Scale(Vec3(0.5f, 0.5f, 0.5f));

	m_vColor.w = 1.0f;

	//Vec3 vPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION) + m_vOffset;
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fEffectTime = 1.5f;

	m_fAlphaWeight = 0.0f;

	m_fCurrTime = m_fSmokeDelay;

	for (_uint i = 0; i < 10; ++i)
	{
		CSmokeRing_Syuriken* pSmoke = CPool<CSmokeRing_Syuriken>::Get_Obj();

		pSmoke->Appear(vPos, 20.0f, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.0f, 0.0f, 0.0f, 0.0f), 0.8f);
	}
}

void CRasenSyurikenSphere::DisAppear()
{
	Set_Active(false);
}


HRESULT CRasenSyurikenSphere::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_LockFree_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_SphereNoise"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_SphereDisolve"),
		TEXT("Com_DisolveTexture"), (CComponent**)&m_pDisolveTexture)))
		return E_FAIL;



	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_CommonSphere";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}


CGameObject* CRasenSyurikenSphere::Clone(void* pArg)
{
	CRasenSyurikenSphere* pInstance = new CRasenSyurikenSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRasenSyurikenSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CRasenSyurikenSphere* CRasenSyurikenSphere::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRasenSyurikenSphere* pInstance = new CRasenSyurikenSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasenSyurikenSphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CRasenSyurikenSphere::Free()
{
	__super::Free();
}
