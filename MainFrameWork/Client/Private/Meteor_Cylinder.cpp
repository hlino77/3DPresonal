#include "stdafx.h"
#include "Meteor_Cylinder.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Pool.h"



CMeteor_Cylinder::CMeteor_Cylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CMeteor_Cylinder::CMeteor_Cylinder(const CMeteor_Cylinder& rhs)
	: CEffect(rhs)
{
}

HRESULT CMeteor_Cylinder::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CMeteor_Cylinder::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Meteor_HemiSphere";

	m_bActive = false;


	m_vColor = Vec4(0.1f, 0.1f, 1.0f, 1.0f);
	m_vBlur = m_vColor;

	//m_vBlur = Vec4(0.07f, 0.3f, 0.63f, 1.0f);

	m_vBlur *= 0.1f;


	m_fAlphaWeight = 1.2f;

	m_vTargetScale = Vec3(31.9f, 1000.0f, 31.9f);


	return S_OK;
}

void CMeteor_Cylinder::Tick(_float fTimeDelta)
{
	m_fEffectTime -= fTimeDelta;
	if (m_fEffectTime <= 0.0f)
		m_bEnd = true;

	Vec3 vScale = m_pTransformCom->Get_Scale();

	if (vScale.x < m_vTargetScale.x)
	{
		Vec3 vAdd(10.0f, 300.0f, 10.0f);

		vScale += vAdd * fTimeDelta;

		if (vScale.x > m_vTargetScale.x)
			vScale = m_vTargetScale;
	}

	m_pTransformCom->Set_Scale(vScale);



	if (m_bEnd)
	{
		m_vColor.w -= 1.0f * fTimeDelta;
		if (m_vColor.w <= 0.0f)
			Set_Active(false);
	}


	Vec3 vPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vPos.y = -30.0f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CMeteor_Cylinder::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CMeteor_Cylinder::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vBlurColor", &m_vBlur, sizeof(Vec4))))
		return S_OK;


	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlphaWeight", &m_fAlphaWeight, sizeof(_float))))
		return S_OK;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;




	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{


		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}



void CMeteor_Cylinder::Appear()
{
	m_bActive = true;

	m_bEnd = false;

	m_vColor.w = 0.3f;


	Vec3 vPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vPos.y = -30.0f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	m_fEffectTime = 4.0f;


	m_pTransformCom->Set_Scale(Vec3(0.01f, 0.1f, 0.01f));
}

void CMeteor_Cylinder::DisAppear()
{
	m_bEnd = true;
}


HRESULT CMeteor_Cylinder::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Default"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_Meteor_Cylinder";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}


CGameObject* CMeteor_Cylinder::Clone(void* pArg)
{
	CMeteor_Cylinder* pInstance = new CMeteor_Cylinder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CMeteor_Cylinder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CMeteor_Cylinder* CMeteor_Cylinder::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CMeteor_Cylinder* pInstance = new CMeteor_Cylinder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMeteor_Cylinder");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMeteor_Cylinder::Free()
{
	__super::Free();
}
