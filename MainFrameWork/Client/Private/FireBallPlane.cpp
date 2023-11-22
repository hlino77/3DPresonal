#include "stdafx.h"
#include "FireBallPlane.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Pool.h"



CFireBallPlane::CFireBallPlane(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CFireBallPlane::CFireBallPlane(const CFireBallPlane& rhs)
	: CEffect(rhs)
{
}

HRESULT CFireBallPlane::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CFireBallPlane::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Meteor_FireBallPlane";

	m_bActive = false;


	m_vColor = Vec4(1.0f, 0.3f, 0.0f, 1.0f);
	m_vBlur = m_vColor;



	Vec3 vScale = Vec3(0.8f, 0.8f, 0.8f);

	m_pTransformCom->Set_Scale(vScale);


	m_fAlphaWeight = 0.0f;



	m_fUVX = 0.0f;
	m_fUVY = 0.0f;


	return S_OK;
}

void CFireBallPlane::Tick(_float fTimeDelta)
{
	Update_Disolve(fTimeDelta);


	m_fUVY += 1.0f * fTimeDelta;

	Vec3 vPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	Vec3 vCamPos = CGameInstance::GetInstance()->Get_CamPosition();
	m_pTransformCom->LookAt(vCamPos);

}

void CFireBallPlane::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CFireBallPlane::Render()
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

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVX", &m_fUVX, sizeof(_float))))
		return S_OK;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fUVY", &m_fUVY, sizeof(_float))))
		return S_OK;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;

	if (FAILED(m_pDisolveTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture2", 0)))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{


		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 7)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}



void CFireBallPlane::Appear()
{
	m_bActive = true;

	m_bEnd = false;

	m_fAlphaWeight = 0.3f;
	m_bDir = true;

	m_fUVX = 0.0f;
	m_fUVY = 0.0f;

}

void CFireBallPlane::DisAppear()
{
	Set_Active(false);
}


HRESULT CFireBallPlane::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FirePlane"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireDisolve"),
		TEXT("Com_DisolveTexture"), (CComponent**)&m_pDisolveTextureCom)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_FireBallPlane";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}

void CFireBallPlane::Update_Disolve(_float fTimeDelta)
{
	if (m_bDir)
	{
		m_fAlphaWeight += 1.0f * fTimeDelta;
		if (m_fAlphaWeight > 0.99f)
		{
			m_fAlphaWeight = 0.99f;
			m_bDir = false;
		}
		
	}
	else
	{
		m_fAlphaWeight -= 1.0f * fTimeDelta;
		if (m_fAlphaWeight < 0.7f)
		{
			m_fAlphaWeight = 0.7f;
			m_bDir = true;
		}
	}
}


CGameObject* CFireBallPlane::Clone(void* pArg)
{
	CFireBallPlane* pInstance = new CFireBallPlane(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CFireBallPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFireBallPlane* CFireBallPlane::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireBallPlane* pInstance = new CFireBallPlane(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CFireBallPlane");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CFireBallPlane::Free()
{
	__super::Free();
}
