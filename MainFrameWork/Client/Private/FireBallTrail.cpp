#include "stdafx.h"
#include "FireBallTrail.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"



CFireBallTrail::CFireBallTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CFireBallTrail::CFireBallTrail(const CFireBallTrail& rhs)
	: CEffect(rhs)
{
}

HRESULT CFireBallTrail::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CFireBallTrail::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_FireBallTrail";

	m_bActive = false;


	m_vColor = Vec4(1.0f, 0.3f, 0.0f, 1.0f);

	//m_vBlur = Vec4(0.07f, 0.3f, 0.63f, 1.0f);



	m_fUVSpeed = 1.0f;


	Vec3 vScale = Vec3(1.0f, 1.0f, 1.0f);

	m_pTransformCom->Set_Scale(vScale);

	

	return S_OK;
}

void CFireBallTrail::Tick(_float fTimeDelta)
{
	m_fUV += m_fUVSpeed * fTimeDelta;


	if (m_bEnd)
	{
		m_vColor.w -= 1.0f * fTimeDelta;
		if (m_vColor.w <= 0.0f)
			Set_Active(false);
	}
	else
		Set_LookAt();


}

void CFireBallTrail::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODELEFFECT_INSTANCE, this);
}

HRESULT CFireBallTrail::Render()
{
	
	return S_OK;
}

HRESULT CFireBallTrail::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
{

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);


	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW))))
		return S_OK;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ))))
		return S_OK;


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture")))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_uint iStride = sizeof(VTXINSTANCE_MODELEFFECT);


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return S_OK;

		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;*/


		if (FAILED(m_pModelCom->Render_Instance(pInstanceBuffer, iSize, m_pShaderCom, i, iStride, 2)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CFireBallTrail::Add_InstanceData(vector<Vec4>& BufferData)
{
	Matrix matWorld = m_pTransformCom->Get_WorldMatrix();

	matWorld._14 = m_fUV;


	BufferData.push_back(Vec4(matWorld.m[0]));
	BufferData.push_back(Vec4(matWorld.m[1]));
	BufferData.push_back(Vec4(matWorld.m[2]));
	BufferData.push_back(Vec4(matWorld.m[3]));
	BufferData.push_back(m_vColor);
	BufferData.push_back(m_vBlur);
}


void CFireBallTrail::Appear()
{
	m_bActive = true;

	m_bEnd = false;

	m_fUV = -1.0f;



	m_vColor.w = 0.8f;
}

void CFireBallTrail::DisAppear()
{
	m_bEnd = true;
}


void CFireBallTrail::Set_LookAt()
{
	Vec3 vPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	Vec3 vLook = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
	vLook.Normalize();
	m_pTransformCom->LookAt(vPos + vLook);
}

HRESULT CFireBallTrail::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ModelEffect_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_FireBallTrail"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_FireBallTrail";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}


CGameObject* CFireBallTrail::Clone(void* pArg)
{
	CFireBallTrail* pInstance = new CFireBallTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CRasengan_RunSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CFireBallTrail* CFireBallTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CFireBallTrail* pInstance = new CFireBallTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CRasengan_RunSmoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CFireBallTrail::Free()
{
	__super::Free();
}
