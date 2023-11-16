#include "stdafx.h"
#include "Lightning_C_01.h"
#include "Client_Defines.h"
#include "Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Pool.h"




CLightning_C_01::CLightning_C_01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CEffect(pDevice, pContext)
{
}

CLightning_C_01::CLightning_C_01(const CLightning_C_01& rhs)
	: CEffect(rhs)
{
}

HRESULT CLightning_C_01::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CLightning_C_01::Initialize(void* pArg)
{
	__super::Initialize(pArg);


	m_szModelName = L"Effect_Lightning_C_01";

	m_bActive = false;


	m_vColor = Vec4(0.84f, 0.95f, 1.0f, 1.0f);
	m_vBlur = Vec4(0.5f, 0.84f, 1.0f, 1.0f);

	//m_vBlur = Vec4(0.07f, 0.3f, 0.63f, 1.0f);


	m_vBlur *= 0.3f;


	m_RandomNumber = mt19937_64((*m_RandomDevice)());
	m_RandomScale = uniform_real_distribution<float>(0.5f, 1.0f);


	m_fUVSpeed = 7.0f;

	return S_OK;
}

void CLightning_C_01::Tick(_float fTimeDelta)
{
	m_fUV -= m_fUVSpeed * fTimeDelta;

	if (m_fUV <= -1.0f)
	{
		Set_Active(false);
		CPool<CLightning_C_01>::Return_Obj(this);
	}
}

void CLightning_C_01::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MODELEFFECT_INSTANCE, this);
}

HRESULT CLightning_C_01::Render()
{

	return S_OK;
}

HRESULT CLightning_C_01::Render_Instance(ID3D11Buffer* pInstanceBuffer, _uint iSize)
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


		if (FAILED(m_pModelCom->Render_Instance(pInstanceBuffer, iSize, m_pShaderCom, i, iStride, 0)))
			return S_OK;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLightning_C_01::Add_InstanceData(vector<Vec4>& BufferData)
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


void CLightning_C_01::Appear(Vec3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_bActive = true;


	Matrix matWorld = XMMatrixIdentity();


	_float fScale = m_RandomScale(m_RandomNumber);

	matWorld._11 *= fScale;
	matWorld._22 *= fScale;
	matWorld._33 *= fScale;


	_float fYaw, fPitch, fRoll;

	fYaw = XMConvertToRadians(rand() % 360);
	fRoll = XMConvertToRadians(rand() % 360);
	fPitch = XMConvertToRadians(rand() % 360);

	Matrix matRot = Matrix::CreateFromYawPitchRoll(fYaw, fPitch, fRoll);


	matWorld *= matRot;
	matWorld.Translation(vPos);

	m_pTransformCom->Set_WorldMatrix(matWorld);

	m_bEnd = false;

	m_fUV = 2.0f;
}

void CLightning_C_01::DisAppear()
{
	m_bEnd = true;
}



HRESULT CLightning_C_01::Ready_Components()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Effect_Lightning"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Lightning"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	///* For.Com_Model */
	wstring strComName = L"Prototype_Component_Model_Lightning_C_01";
	if (FAILED(__super::Add_Component(LEVEL_STATIC, strComName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



	Safe_Release(pGameInstance);



	return S_OK;
}


CGameObject* CLightning_C_01::Clone(void* pArg)
{
	CLightning_C_01* pInstance = new CLightning_C_01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Cloned : CLightning_C_01");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CLightning_C_01* CLightning_C_01::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLightning_C_01* pInstance = new CLightning_C_01(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLightning_C_01");
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLightning_C_01::Free()
{
	__super::Free();
}
